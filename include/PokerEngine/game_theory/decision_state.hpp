#ifndef POKER_ENGINE_GAME_THEORY_DECISION_STATE_HPP
#define POKER_ENGINE_GAME_THEORY_DECISION_STATE_HPP

#include <vector>
#include <numeric>

#include "PokerEngine/core/card.hpp"
#include "PokerEngine/core/hand.hpp"
#include "PokerEngine/core/board.hpp"
#include "PokerEngine/core/stack.hpp"
#include "PokerEngine/core/deck.hpp"
#include "PokerEngine/core/pot.hpp"

namespace PokerEngine::GameTheory {
    
enum class Street {PREFLOP, FLOP, TURN, RIVER};

struct PlayerState {
    int id;
    Core::Stack stack;
    Core::Hand hand;
    int currentBet;
};

struct DecisionState {
    int currentPlayerIndex;
    int lastAggressorIndex = -1; // -1 means no bet/raise has occurred
    int firstToActIndex = 0;     // index of the first player to act in the round
    Street street;
    std::vector<PlayerState> activePlayers;
    Core::Pot pot;
    Core::Board board;
    Core::Deck deck;
    
    bool terminal = false;

    bool isChanceNode() const {
        switch (street) {
            case Street::FLOP: return board.size() < 3;
            case Street::TURN: return board.size() < 4;
            case Street::RIVER: return board.size() < 5;
            default: return false;
        }
    }

    bool isTerminal() const {
        return terminal;
    }

    int nextPlayerIndex() {
        return (currentPlayerIndex + 1) % activePlayers.size();
    }

    void removePlayer(int player_index) {
        pot.addContribution(player_index, activePlayers[player_index].currentBet);
        activePlayers.erase(activePlayers.begin() + player_index);
    }

    void goToNextRound() {
        switch (street) {
            case Street::PREFLOP: {
                street = Street::FLOP; 
                board.add(deck.draw(3));
                break;
            }
            case Street::FLOP: {
                street = Street::TURN;
                board.add(deck.draw(1));
                break;
            }
            case Street::TURN: {
                street = Street::RIVER; 
                board.add(deck.draw(1));
                break;
            }
            case Street::RIVER: terminal = true; break;
        }

        for(auto& p : activePlayers) {
            pot.addContribution(p.id, p.currentBet);
            p.currentBet = 0;
        }

        currentPlayerIndex = 0;
        firstToActIndex = 0;
        lastAggressorIndex = -1;
    }

    /**
     * @brief Check if a new card should be dealt (chance node condition)
     */
    bool needsToDealNextCard() const {
        // A new card should be dealt *if*:
        // - the betting round is complete
        // - we are not terminal
        // - and the next street card hasn’t yet been dealt
        if (terminal) return false;

        switch (street) {
            case Street::PREFLOP:
                // after PREFLOP betting ends → deal FLOP
                return board.size() < 3;
            case Street::FLOP:
                // after FLOP betting ends → deal TURN
                return board.size() < 4;
            case Street::TURN:
                // after TURN betting ends → deal RIVER
                return board.size() < 5;
            case Street::RIVER:
                return false;
        }
        return false;
    }

    /**
     * @brief Advance to next street and deal appropriate cards.
     * Called from chance node expansion.
     */
    void advanceStreetAfterDeal() {
        switch (street) {
            case Street::PREFLOP: {
                street = Street::FLOP;
                // deal remaining cards up to 3
                while (board.size() < 3) board.add(deck.draw());
                break;
            }
            case Street::FLOP: {
                street = Street::TURN;
                if (board.size() < 4) board.add(deck.draw());
                break;
            }
            case Street::TURN: {
                street = Street::RIVER;
                if (board.size() < 5) board.add(deck.draw());
                break;
            }
            case Street::RIVER:
                terminal = true;
                break;
        }

        // Reset round-level state
        for (auto& p : activePlayers) {
            pot.addContribution(p.id, p.currentBet);
            p.currentBet = 0;
        }
        currentPlayerIndex = 0;
    }
};
}

#endif