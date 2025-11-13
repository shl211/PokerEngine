#ifndef POKER_ENGINE_EVALUATOR_HAND_EVALUATOR_STRATEGY_HPP
#define POKER_ENGINE_EVALUATOR_HAND_EVALUATOR_STRATEGY_HPP

#include <optional>
#include <utility>
#include <vector>
#include <functional>

#include "PokerEngine/evaluator/hand_rank.hpp"
#include "PokerEngine/evaluator/detail/bit_mask.hpp"

namespace PokerEngine::Evaluator {

using HandEvalFn = std::function<std::optional<HandRank>(detail::HandMask mask)>;

class HandEvaluationStrategy {
public:
    HandEvaluationStrategy(std::vector<HandEvalFn> evaluation_order)
        : evaluation_order_(evaluation_order) {}

    auto begin() const { return evaluation_order_.begin(); }
    auto end() const { return evaluation_order_.end(); }

private:
    std::vector<HandEvalFn> evaluation_order_; 
};

}

#endif