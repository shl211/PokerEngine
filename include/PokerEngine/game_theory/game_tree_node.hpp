#ifndef POKER_ENGINE_GAME_THEORY_GAME_TREE_NODE_HPP
#define POKER_ENGINE_GAME_THEORY_GAME_TREE_NODE_HPP

#include <vector>
#include <memory>

#include "PokerEngine/game_theory/decision_state.hpp"

namespace PokerEngine::GameTheory {
enum class NodeType { Decision, Chance, Terminal };

enum class ActionType {
    FOLD,
    CHECK,
    CALL,
    BET,
    RAISE,
    DEAL
};

struct GameTreeNode;

struct ActionEdge {
    ActionType action;
    std::shared_ptr<GameTreeNode> next;
};

struct GameTreeNode {
    DecisionState state;
    NodeType type = NodeType::Decision;
    std::vector<ActionEdge> children;

    void addChild(ActionType action, std::shared_ptr<GameTreeNode> next) {
        children.push_back({ action, std::move(next) });
    }
};

}

#endif