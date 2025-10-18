#ifndef POKER_ENGINE_EVALUATOR_HAND_EVALUATOR_HPP
#define POKER_ENGINE_EVALUATOR_HAND_EVALUATOR_HPP

#include <vector>

#include "PokerEngine/core/card.hpp"
#include "PokerEngine/evaluator/hand_rank.hpp"

namespace PokerEngine::Evaluator {

class HandEvaluator {
public:
    HandRank evaluate(const std::vector<Core::Card>&);

};
}

#endif