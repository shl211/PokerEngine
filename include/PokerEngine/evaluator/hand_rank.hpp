#ifndef POKER_ENGINE_EVALUATOR_HAND_RANK_HPP
#define POKER_ENGINE_EVALUATOR_HAND_RANK_HPP

#include <vector>

namespace PokerEngine::Evaluator {

enum class Category {
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
    Category category;
    std::vector<int> tiebreakers;
    
    auto operator<=>(const HandRank&) const = default;
};
}

#endif