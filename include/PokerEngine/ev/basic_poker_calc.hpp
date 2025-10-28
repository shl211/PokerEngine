#ifndef POKER_ENGINE_EV_BASIC_POKER_CALC_HPP
#define POKER_ENGINE_EV_BASIC_POKER_CALC_HPP

#include "PokerEngine/ev/ev_calculator.hpp"

namespace PokerEngine::EV {

/**
 * @brief Single street heads up ev calculation, assuming you are caling a raise
 */
static double calcCallEV(double equity, int pot, int call_amount) {
    WeightedOutcome wo1 {.weight = equity, .outcome = pot + call_amount};
    WeightedOutcome wo2 {.weight = 1.0 - equity, .outcome = -call_amount};
    return calculateEV({wo1, wo2});
}

/**
 * @brief Single street heads up ev calculation, assuming you raise and get called
 */
static double calcRaiseEV(double equity, int pot, int raise_amount, int opponent_call) {
    WeightedOutcome wo1 {.weight = equity, .outcome = pot + raise_amount + opponent_call};
    WeightedOutcome wo2 {.weight = 1.0 - equity, .outcome = -raise_amount};
    return calculateEV({wo1, wo2});
}

static constexpr double calcFoldEV() {
    return 0.0;
}
}

#endif