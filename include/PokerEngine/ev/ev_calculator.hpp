#ifndef POKER_ENGINE_EV_EV_CALCULATOR_HPP
#define POKER_ENGINE_EV_EV_CALCULATOR_HPP

#include <vector>
#include <numeric>
#include <utility>

namespace PokerEngine::EV {

struct WeightedOutcome {
    double weight;
    double outcome;
};

namespace {
    struct Accumulator {
        double weighted_sum = 0.0;
        double total_weight = 0.0;
    };
}

double calculateEV(const std::vector<WeightedOutcome>& outcomes) {
    Accumulator acc = std::accumulate(
        outcomes.begin(), outcomes.end(),
        Accumulator{},
        [](Accumulator acc, const WeightedOutcome& w_outcome) {
            acc.weighted_sum += w_outcome.weight * w_outcome.outcome;
            acc.total_weight += w_outcome.weight;
            return acc;
        }
    );

    return acc.total_weight != 0.0 ? acc.weighted_sum / acc.total_weight : 0.0;
}
}

#endif