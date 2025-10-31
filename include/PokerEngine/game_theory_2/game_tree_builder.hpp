#ifndef POKER_ENGINE_GAME_THEORY_2_GAME_TREE_BUILDER_HPP
#define POKER_ENGINE_GAME_THEORY_2_GAME_TREE_BUILDER_HPP

#include <memory>
#include <vector>
#include "PokerEngine/game_theory_2/game_tree_node.hpp"

namespace PokerEngine::GameTheory2 {

struct GameTreeBuilderConfig {
    std::vector<double> betFractions = {0.25, 0.5, 1.0};
    int maxDepth = 3;
};

class GameTreeBuilder {
public:
    explicit GameTreeBuilder(GameTreeBuilderConfig config)
        : config_(std::move(config)) {}

    std::shared_ptr<GameTreeNode> buildTree(const DecisionState& root);

private:
    GameTreeBuilderConfig config_;

    std::shared_ptr<GameTreeNode> expandNode(const DecisionState& state, int depth);
    std::vector<ActionType> getLegalActions(const DecisionState& state);
    DecisionState simulateAction(const DecisionState& state, ActionType action);
};

namespace {
    template< class Iterator>
    std::vector<Core::Card> n_choose_k_combos(Iterator begin, Iterator end, int k) {
        //TODO, write in a util class
        return {};
    } 

    inline std::vector<DecisionState> dealAllPossibleNextCards(const DecisionState& curr, int cards_to_deal, Street next_street) {
        std::vector<Core::Card> possible_next_cards = n_choose_k_combos(curr.deck.begin(), curr.deck.end(), cards_to_deal);

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
            node->addChild(ActionType::DEAL, expandNode(next_state, depth + 1));
        }

        return node;
    }

    node->type = NodeType::Decision;

    for (auto action : getLegalActions(state)) {
        auto nextState = simulateAction(state, action);
        node->addChild(action, expandNode(nextState, depth + 1));
    }

    return node;
}

}

#endif
