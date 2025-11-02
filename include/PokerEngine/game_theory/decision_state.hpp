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

    bool isTerminal() const {
        int active_players = std::count_if(players.begin(), players.end(),
            [](const auto& p) { return !p.folded; } 
        );

        return active_players <= 1 ||
            (round.street == Street::RIVER && isRoundEnded()); 
        }

    bool isRoundEnded() const {
        for(auto& p : players) {
            if(!p.folded && p.stillToAct) return false;
        }

        return true;
    }

    void resetForNextRound(int numPlayers) {
        round.resetForNextRound(numPlayers);
        for(auto& p : players) {
            if(!p.currentBet != 0) {
                pot.addContribution(p.id, p.currentBet);
                p.currentBet = 0;
            }
            if(!p.folded && !p.stack.empty()) p.stillToAct = true;
        }
    }

    void advancePlayerTurn() {
        int num_players = players.size();
        int start_index = round.currentPlayerIndex;
        do {
            round.advanceTurn(num_players);
            // stop if we looped over all players
            if(round.currentPlayerIndex == start_index) break;
        } while(players[round.currentPlayerIndex].folded);
    }
};
}

#endif