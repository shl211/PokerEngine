#include <gtest/gtest.h>

#include "PokerEngine/ev/ev_calculator.hpp"

using namespace PokerEngine::EV;

TEST(EVCalc, Test1) {
    WeightedOutcome wo1 {.weight = 1.0, .outcome = 100.0};
    WeightedOutcome wo2 {.weight = 1.0, .outcome = -100.0};

    double ev = calculateEV({wo1, wo2});
    ASSERT_DOUBLE_EQ(ev, 0.0);
}

TEST(EVCalc, Test2) {
    WeightedOutcome wo1 {.weight = 1.0, .outcome = 100.0};
    WeightedOutcome wo2 {.weight = 3.0, .outcome = 0.0};

    double ev = calculateEV({wo1, wo2});
    ASSERT_DOUBLE_EQ(ev, 25);
}