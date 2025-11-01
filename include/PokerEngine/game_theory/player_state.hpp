#ifndef POKER_ENGINE_GAME_THEORY_PLAYER_STATE_HPP
#define POKER_ENGINE_GAME_THEORY_PLAYER_STATE_HPP

#include "PokerEngine/core/stack.hpp"
#include "PokerEngine/core/hand.hpp"

namespace PokerEngine::GameTheory {

using PlayerId = int;

struct PlayerState {
    PlayerId id;
    Core::Stack stack;
    Core::Hand hand;
    int currentBet = 0;
    bool folded = false;
    bool stillToAct = true;
};

}

#endif