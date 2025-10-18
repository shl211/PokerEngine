#ifndef POKER_ENGINE_EVALUATOR_HAND_RANK_HPP
#define POKER_ENGINE_EVALUATOR_HAND_RANK_HPP

#include <vector>
#include <cstdint>
#include "PokerEngine/core/card.hpp"

namespace PokerEngine::Evaluator {

enum class HandType {
    HighCard,
    OnePair,
    TwoPair,
    ThreeOfAKind,
    Straight,
    Flush,
    FullHouse,
    FourOfAKind,
    StraightFlush,
    RoyalFlush
};
    
struct HandRank {
    HandType handtype;
    std::vector<PokerEngine::Core::Card> besthand;
    uint64_t score = 0;             // numerical value for comparison
};

constexpr inline auto operator<=>(const HandRank& a, const HandRank& b) {
    return a.score <=> b.score;
}

}

#endif