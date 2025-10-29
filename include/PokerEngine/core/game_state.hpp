#ifndef POKER_ENGINE_CORE_GAME_STATE_HPP
#define POKER_ENGINE_CORE_GAME_STATE_HPP

#include <vector>

#include "PokerEngine/core/board.hpp"
#include "PokerEngine/core/pot.hpp"
#include "PokerEngine/core/player_state.hpp"

namespace PokerEngine::Core {

struct GameState {
    std::vector<PlayerState> players;
    Board communityCards;
    Pot pot;
};

}

#endif