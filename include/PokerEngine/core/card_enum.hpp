#ifndef POKER_ENGINE_CORE_CARD_ENUM_HPP
#define POKER_ENGINE_CORE_CARD_ENUM_HPP

#include <cstdint>

namespace PokerEngine::Core {

enum class Suit : uint8_t { Hearts, Diamonds, Clubs, Spades };
enum class Rank : uint8_t {
    Two = 2, Three, Four, Five, Six, Seven, Eight,
    Nine, Ten, Jack, Queen, King, Ace
};

}

#endif