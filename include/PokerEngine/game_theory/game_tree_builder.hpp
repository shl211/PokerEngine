#ifndef POKER_ENGINE_GAME_THEORY_GAME_TREE_BUILDER_HPP
#define POKER_ENGINE_GAME_THEORY_GAME_TREE_BUILDER_HPP

#include <memory>
#include <vector>

#include "PokerEngine/game_theory/game_tree_node.hpp"
#include "PokerEngine/game_theory/decision_state.hpp"

namespace PokerEngine::GameTheory {

struct GameTreeBuilderConfig {
    std::vector<double> betSizes; // e.g., {0.25, 0.5, 1.0} as fraction of pot
    int maxDepth;                 // optional limit to avoid huge trees
};

struct Action {
    ActionType type;
    int amount; // 0 for fold/call/check
};

class GameTreeBuilder {
public:
    GameTreeBuilder(const GameTreeBuilderConfig& config);

    std::shared_ptr<GameTreeNode> buildTree(const DecisionState& root_state);

private:
    GameTreeBuilderConfig config_;

    /**
     * @brief Recursively expand a node
     */
    std::shared_ptr<GameTreeNode> expandNode(const DecisionState& state, int depth);

    /**
     * @brief Generate legal actions for a given state
     */
    std::vector<ActionType> getLegalActions(const DecisionState& state);

    /**
     * @brief Generate next states for a given action
     */
    DecisionState simulateAction(const DecisionState& state, Action action);

};

std::vector<ActionType> GameTreeBuilder::getLegalActions(const DecisionState& state) {
    std::vector<ActionType> actions;
    const auto& player = state.activePlayers[state.currentPlayerIndex];

    if (state.activePlayers.size() > 1)
        actions.push_back(ActionType::FOLD);

    int max_bet = std::max_element(
        state.activePlayers.begin(), state.activePlayers.end(),
        [](const PlayerState& p1, const PlayerState& p2) { return p1.currentBet < p2.currentBet; }
    )->currentBet;
    
    if (player.currentBet < max_bet)
        actions.push_back(ActionType::CALL);
    else
        actions.push_back(ActionType::CHECK);

    // Simple logic: allow BET/RAISE if player has chips
    if (player.stack.chips() > 0) {
        actions.push_back(ActionType::BET);
        if (player.currentBet > 0) {
            actions.push_back(ActionType::RAISE);
        }
    }

    return actions;
}

std::shared_ptr<GameTreeNode> GameTreeBuilder::expandNode(const DecisionState& state, int depth) {
    auto node = std::make_shared<GameTreeNode>();
    node->state = state;

    // --- new branch: chance node handling ---
    if (state.isChanceNode()) {
        node->type = NodeType::Chance;

        for (const auto& card : state.deck) {
            DecisionState next = state;
            next.board.add(card);
            next.deck.remove(card);
            auto next_node = expandNode(next, depth + 1);
            node->addChild(ActionType::DEAL, next_node);
        }

        return node;
    }

    // --- terminal or depth limit ---
    if (state.isTerminal() || depth >= config_.maxDepth) {
        node->type = NodeType::Terminal;
        return node;
    }

    // --- normal decision logic (unchanged) ---
    auto legal_actions = getLegalActions(state);

    for (auto action_type : legal_actions) {
        if (action_type == ActionType::BET || action_type == ActionType::RAISE) {
            for (double fraction : config_.betSizes) {
                int pot_for_bet = state.pot.getTotal();
                for (const auto &p : state.activePlayers) pot_for_bet += p.currentBet;
                int bet_amount = static_cast<int>(fraction * pot_for_bet);

                Action action{action_type, bet_amount};
                auto next_state = simulateAction(state, action);
                node->addChild(action_type, expandNode(next_state, depth + 1));
            }
        } else {
            Action action{action_type, 0};
            auto next_state = simulateAction(state, action);
            node->addChild(action_type, expandNode(next_state, depth + 1));
        }
    }

    return node;
}

GameTreeBuilder::GameTreeBuilder(const GameTreeBuilderConfig& config)
    : config_(config) {}

std::shared_ptr<GameTreeNode> GameTreeBuilder::buildTree(const DecisionState& root_state) {
    return expandNode(root_state, 0);
}

void advanceIfRoundComplete(DecisionState& state) {
    if (state.activePlayers.empty() || state.isTerminal()) return;

    int max_bet = 0;
    for (const auto &p : state.activePlayers) 
        max_bet = std::max(max_bet, p.currentBet);

    bool all_equal = true;
    for (const auto &p : state.activePlayers) {
        if (p.currentBet != max_bet) { all_equal = false; break; }
    }

    // Case 1: betting occurred
    if (state.lastAggressorIndex != -1) {
        if (all_equal && state.currentPlayerIndex == state.lastAggressorIndex) {
            state.goToNextRound();
        }
    }
    // Case 2: everyone checked through
    else {
        bool everyone_checked = true;
        for (const auto &p : state.activePlayers) {
            if (p.currentBet != 0) { everyone_checked = false; break; }
        }

        if (everyone_checked) {
            // Advance only if we've looped back to the first player
            int next_index = state.nextPlayerIndex();
            if (next_index == state.firstToActIndex) {
                state.goToNextRound();
            }
        }
    }
}

DecisionState GameTreeBuilder::simulateAction(const DecisionState& state, Action action) {
    DecisionState next_state = state;
    PlayerState &player = next_state.activePlayers[next_state.currentPlayerIndex];

    switch (action.type) {
        case ActionType::FOLD:
            next_state.removePlayer(next_state.currentPlayerIndex);
            if (next_state.activePlayers.size() == 1) {
                next_state.terminal = true;
            } else {
                next_state.currentPlayerIndex %= next_state.activePlayers.size();
            }
            break;

        case ActionType::CHECK:
            next_state.currentPlayerIndex = next_state.nextPlayerIndex();
            break;

        case ActionType::CALL: {
            int max_bet = 0;
            for (const auto &p : next_state.activePlayers)
                max_bet = std::max(max_bet, p.currentBet);

            int to_call = std::min(max_bet - player.currentBet, player.stack.chips());
            player.currentBet += to_call;
            player.stack.removeChips(to_call);

            next_state.currentPlayerIndex = next_state.nextPlayerIndex();
            break;
        }

        case ActionType::BET:
        case ActionType::RAISE: {
            int bet_amount = std::min(static_cast<int>(action.amount), player.stack.chips());
            player.currentBet += bet_amount;
            player.stack.removeChips(bet_amount);
            
            next_state.lastAggressorIndex = next_state.currentPlayerIndex;
            next_state.currentPlayerIndex = next_state.nextPlayerIndex();
            break;
        }
    }

    advanceIfRoundComplete(next_state);
    return next_state;
}


}

#endif