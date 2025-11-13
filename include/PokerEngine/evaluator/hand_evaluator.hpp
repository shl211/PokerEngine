#ifndef POKER_ENGINE_EVALUATOR_HAND_EVALUATOR_HPP
#define POKER_ENGINE_EVALUATOR_HAND_EVALUATOR_HPP

#include <vector>
#include <algorithm>
#include <optional>

#include "PokerEngine/core/card.hpp"
#include "PokerEngine/evaluator/hand_rank.hpp"
#include "PokerEngine/evaluator/detail/bit_mask.hpp"
#include "PokerEngine/evaluator/hand_evaluator_strategy.hpp"
#include "PokerEngine/evaluator/nlh_evaluator_strategy.hpp"

namespace PokerEngine::Evaluator {
class HandEvaluator {
public:
    HandEvaluator(HandEvaluationStrategy hand_evaluator)
        : hand_eval_strategy_(hand_evaluator) {}
    HandRank evaluate(const std::vector<Core::Card>&) const;
    
private:
    HandEvaluationStrategy hand_eval_strategy_;
};
    
HandRank HandEvaluator::evaluate(const std::vector<Core::Card>& cards) const {
    detail::HandMask mask = detail::cards_bitmask(cards);
    for(auto strat : hand_eval_strategy_) {
        if(auto res = strat(mask) ; res) {
            return *res;
        }
    }

    HandEvalFn default_strat = *(hand_eval_strategy_.end());
    return *default_strat(mask);
}
}

#endif