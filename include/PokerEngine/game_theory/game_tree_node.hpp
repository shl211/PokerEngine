#ifndef POKER_ENGINE_GAME_THEORY_NODE_HPP
#define POKER_ENGINE_GAME_THEORY_NODE_HPP

#include <memory>
#include <vector>

#include "PokerEngine/game_theory/decision_state.hpp"
#include "PokerEngine/core/game_state.hpp"

namespace PokerEngine::GameTheory {

enum class ActionType {
    FOLD,
    CHECK,
    CALL,
    BET,
    RAISE
};

struct GameTreeNode;

struct ActionEdge {
    ActionType action;
    std::shared_ptr<GameTreeNode> nextNode;
};

struct GameTreeNode {
    DecisionState state;
    std::vector<ActionEdge> children;

    bool isTerminal() const { return state.terminal; }

    void addChild(ActionType action, std::shared_ptr<GameTreeNode> next) {
        children.push_back({ action, std::move(next) });
    }
};
}

#endif