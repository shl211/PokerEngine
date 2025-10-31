#ifndef POKER_ENGINE_GAME_THEORY_2_DECISION_STATE_HPP
#define POKER_ENGINE_GAME_THEORY_2_DECISION_STATE_HPP

#include <vector>
#include "PokerEngine/game_theory_2/player_state.hpp"
#include "PokerEngine/game_theory_2/round_state.hpp"
#include "PokerEngine/core/pot.hpp"
#include "PokerEngine/core/board.hpp"
#include "PokerEngine/core/deck.hpp"


namespace PokerEngine::GameTheory2 {

struct DecisionState {
    RoundState round;
    std::vector<PlayerState> players;
    Core::Pot pot;
    Core::Board board;
    Core::Deck deck;
    bool terminal = false;

    bool isChanceNode() const {
        switch (round.street) {
            case Street::PREFLOP: return board.size() < 3;
            case Street::FLOP:    return board.size() < 4;
            case Street::TURN:    return board.size() < 5;
            case Street::RIVER:   return false;
        }
        return false;
    }

    bool isTerminal() const { return terminal; }
};
}

#endif