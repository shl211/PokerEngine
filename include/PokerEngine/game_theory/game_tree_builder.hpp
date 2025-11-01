#ifndef POKER_ENGINE_GAME_THEORY_GAME_TREE_BUILDER_HPP
#define POKER_ENGINE_GAME_THEORY_GAME_TREE_BUILDER_HPP

#include <memory>
#include <vector>
#include <algorithm>
#include "PokerEngine/utils/math.hpp"
#include "PokerEngine/game_theory/game_tree_node.hpp"

namespace PokerEngine::GameTheory {

struct GameTreeBuilderConfig {
    std::vector<double> betFractions = {0.25, 0.5, 1.0};
    int maxDepth = 3;
};

struct Action {
    ActionType type;
    int amount = 0; //0 for fold, call, check
};

class GameTreeBuilder {
public:
    explicit GameTreeBuilder(GameTreeBuilderConfig config)
        : config_(std::move(config)) {}

    std::shared_ptr<GameTreeNode> buildTree(const DecisionState& root);

private:
    GameTreeBuilderConfig config_;

    std::shared_ptr<GameTreeNode> expandNode(const DecisionState& state, int depth);
    std::vector<Action> getLegalActions(const DecisionState& state);
    DecisionState simulateAction(const DecisionState& state, Action action);
};

namespace {
    inline std::vector<DecisionState> dealAllPossibleNextCards(const DecisionState& curr, int cards_to_deal, Street next_street) {
        std::vector<std::vector<Core::Card>> possible_next_cards = Utils::generate_combinations(curr.deck.begin(), curr.deck.end(), cards_to_deal);

        std::vector<DecisionState> next_states;
        next_states.reserve(possible_next_cards.size());

        for(const auto& card : possible_next_cards) {
            auto next_state = curr;
            next_state.deck.remove(card);
            next_state.board.add(card);
            next_state.round.street = next_street;
            next_states.push_back(std::move(next_state));
        }
        
        return next_states;
    }
}

std::shared_ptr<GameTreeNode> GameTreeBuilder::buildTree(const DecisionState& root) {
    return expandNode(root, 0);
}

std::shared_ptr<GameTreeNode> GameTreeBuilder::expandNode(const DecisionState& state, int depth) {
    auto node = std::make_shared<GameTreeNode>();
    node->state = state;

    if (state.isTerminal() || depth >= config_.maxDepth) {
        node->type = NodeType::Terminal;
        return node;
    }

    if (state.isChanceNode()) {
        node->type = NodeType::Chance;

        std::vector<DecisionState> next_states = [&state]() {
            switch (state.round.street) {
                case Street::PREFLOP:   return dealAllPossibleNextCards(state, 3, Street::FLOP);
                case Street::FLOP:      return dealAllPossibleNextCards(state, 1, Street::TURN);
                case Street::TURN:      return dealAllPossibleNextCards(state, 1, Street::RIVER);
                default:                return std::vector<DecisionState>();
            }
        } ();
        
        
        for (auto& next_state : next_states) {
            next_state.round.resetForNextRound(next_state.players.size());
            node->addChild(ActionType::DEAL, expandNode(next_state, depth + 1));
        }

        return node;
    }

    node->type = NodeType::Decision;

    for (auto action : getLegalActions(state)) {
        auto nextState = simulateAction(state, action);
        node->addChild(action.type, expandNode(nextState, depth + 1));
    }

    return node;
}

std::vector<Action> GameTreeBuilder::getLegalActions(const DecisionState& state) {
    std::vector<Action> actions;

    if(state.players.empty()) return actions;

    const auto& player = state.players[state.round.currentPlayerIndex];

    if(player.folded) return actions;

    int max_bet = 0;
    for(const auto& p : state.players) {
        if(!p.folded) max_bet = std::max(max_bet, p.currentBet);
    }

    if(player.currentBet < max_bet) {
        actions.push_back(Action{.type = ActionType::FOLD});
        actions.push_back(Action{.type = ActionType::CALL});

        bool gone_all_in = false; //do not make duplicate all in bets
        for(auto bet_size : config_.betFractions) {
            int bet = bet_size * state.pot.getTotal();
            if(bet <= player.currentBet || gone_all_in) {
                continue;
            }

            if(!player.stack.canAfford(bet)) {
                gone_all_in = true;
                bet = player.stack.chips();
            }

            actions.push_back(Action{ .type = ActionType::RAISE, .amount = bet});
        }
    } else {
        actions.push_back(Action {.type = ActionType::CHECK});
        
        bool gone_all_in = false; //do not make duplicate all in bets
        bool gone_min_size = false;//do not make duplicate min bets
        int min_bet = 1;
        for(auto bet_size : config_.betFractions) {
            int bet = std::max(
                static_cast<int>(bet_size * state.pot.getTotal()), min_bet);

            if(bet <= player.currentBet || gone_all_in || gone_min_size) {
                continue;
            }

            if(bet == min_bet) {
                gone_min_size = true; 
            }

            if(!player.stack.canAfford(bet)) {
                gone_all_in = true;
                bet = player.stack.chips();
            }

            actions.push_back( Action{ .type = ActionType::BET, .amount = bet });
        }
    }

    return actions;
}

DecisionState GameTreeBuilder::simulateAction(const DecisionState& state, Action action) {
    DecisionState next_state = state;
    auto& player = next_state.players[next_state.round.currentPlayerIndex];

    switch(action.type) {
        case ActionType::FOLD:
            player.folded = true;
            player.stillToAct = false;
            break;
        case ActionType::CALL: {
            int max_bet = 0;
            for (const auto& p : next_state.players) {
                if (!p.folded) max_bet = std::max(max_bet, p.currentBet);
            }
            
            int to_call = max_bet - player.currentBet;
            to_call = std::min(to_call, player.stack.chips());
            player.currentBet += to_call;
            player.stack.removeChips(to_call);
            player.stillToAct = false;
            break;
        }
        case ActionType::BET:
        case ActionType::RAISE: {
            int max_bet = 0;
            for (const auto& p : next_state.players) {
                if (!p.folded) max_bet = std::max(max_bet, p.currentBet);
            }
            
            int bet_amount = max_bet + std::min(player.stack.chips(), action.amount);
            bet_amount -= player.currentBet;
            
            player.currentBet += bet_amount;
            player.stack.removeChips(bet_amount);
            
            next_state.round.lastAggressorIndex = next_state.round.currentPlayerIndex;

            for(auto& p : next_state.players) {
                if(p.id != player.id && !p.folded){
                    p.stillToAct = true;
                }
            }
            player.stillToAct = false;

            break;
        }
        case ActionType::CHECK:
            player.stillToAct = false;
            break;
        default:
            break;
    }

    // --- Advance to next active player ---
    int num_players = next_state.players.size();
    int start_index = next_state.round.currentPlayerIndex;
    do {
        next_state.round.advanceTurn(num_players);
        // stop if we looped over all players
        if(next_state.round.currentPlayerIndex == start_index) break;
    } while(next_state.players[next_state.round.currentPlayerIndex].folded);
    
    // --- Check if game is terminal ---
    int active_players = 0;
    for (const auto& p : next_state.players) {
        if(!p.folded) {
            active_players++;
        }
    }

    if(active_players <= 1 || 
        (next_state.round.street == Street::RIVER && next_state.isRoundEnded())) {
        next_state.terminal = true;
    } 

    return next_state;
}





}

#endif
