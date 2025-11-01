#ifndef POKER_ENGINE_GAME_THEORY_DECISION_STATE_HPP
#define POKER_ENGINE_GAME_THEORY_DECISION_STATE_HPP

#include <vector>
#include "PokerEngine/game_theory/player_state.hpp"
#include "PokerEngine/game_theory/round_state.hpp"
#include "PokerEngine/core/pot.hpp"
#include "PokerEngine/core/board.hpp"
#include "PokerEngine/core/deck.hpp"


namespace PokerEngine::GameTheory {

struct DecisionState {
    RoundState round;
    std::vector<PlayerState> players;
    Core::Pot pot;
    Core::Board board;
    Core::Deck deck;
    bool terminal = false;

    bool isChanceNode() const {
        if(terminal || !isRoundEnded()) return false;

        switch (round.street) {
            case Street::PREFLOP: return board.size() < 3;
            case Street::FLOP:    return board.size() < 4;
            case Street::TURN:    return board.size() < 5;
            case Street::RIVER:   return false;
        }
        
        return false;
    }

    bool isTerminal() const { return terminal; }

    bool isRoundEnded() const {
        for(auto& p : players) {
            if(!p.folded && p.stillToAct) return false;
        }

        return true;
    }
};
}

#endif