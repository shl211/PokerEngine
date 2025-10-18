#ifndef POKER_ENGINE_EVALUATOR_HAND_RANK_HPP
#define POKER_ENGINE_EVALUATOR_HAND_RANK_HPP

#include <vector>
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
    
    auto operator<=>(const HandRank&) const = default;
};

}

#endif