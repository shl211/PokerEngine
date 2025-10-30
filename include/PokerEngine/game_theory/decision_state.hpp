#ifndef POKER_ENGINE_GAME_THEORY_DECISION_STATE_HPP
#define POKER_ENGINE_GAME_THEORY_DECISION_STATE_HPP

#include <vector>

#include "PokerEngine/core/card.hpp"
#include "PokerEngine/core/hand.hpp"
#include "PokerEngine/core/board.hpp"
#include "PokerEngine/core/stack.hpp"

namespace PokerEngine::GameTheory {
    
enum class Street {PREFLOP, FLOP, TURN, RIVER};

struct DecisionState {
    int currentPlayerIndex;
    Street street;
    std::vector<int> activePlayers;
    std::vector<Core::Stack> stackSizes;
    std::vector<int> currentBets;
    int potSize;
    Core::Board board;
    std::vector<Core::Hand> holeCards;

    bool terminal = false;

    bool isChanceNode() const {
        return street == Street::FLOP || street == Street::TURN || street == Street::RIVER;
    }

    bool isTerminal() const {
        return terminal;
    }
};

}

#endif