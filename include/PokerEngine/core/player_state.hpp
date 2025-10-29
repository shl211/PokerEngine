#ifndef POKER_ENGINE_CORE_PLAYER_STATE_HPP
#define POKER_ENGINE_CORE_PLAYER_STATE_HPP

#include "PokerEngine/core/hand.hpp"
#include "PokerEngine/core/stack.hpp"

namespace PokerEngine::Core {

struct PlayerState {
    int id;
    Stack stack;
    Hand holeCards;
    bool hasFolded { false };
    bool isAllIn { false };
};
}

#endif