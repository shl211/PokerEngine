#ifndef POKER_ENGINE_GAME_THEORY_2_ROUND_STATE_HPP
#define POKER_ENGINE_GAME_THEORY_2_ROUND_STATE_HPP

#include <optional>

namespace PokerEngine::GameTheory2 {

enum class Street { PREFLOP, FLOP, TURN, RIVER };

struct RoundState {
    Street street;
    int currentPlayerIndex = 0;
    std::optional<int> lastAggressorIndex;
    int firstToActIndex = 0;

    bool roundFinished = false;

    void advanceTurn(int numPlayers) {
        currentPlayerIndex = (currentPlayerIndex + 1) % numPlayers;
    }

    void resetForNextRound(int numPlayers) {
        roundFinished = false;
        currentPlayerIndex = firstToActIndex % numPlayers;
        lastAggressorIndex.reset();
    }
};

}

#endif