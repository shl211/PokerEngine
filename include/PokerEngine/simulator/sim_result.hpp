#ifndef POKER_ENGINE_SIMULATOR_SIM_RESULT_HPP
#define POKER_ENGINE_SIMULATOR_SIM_RESULT_HPP

namespace PokerEngine::Simulator {

struct SimResult {
    double win = 0.0;
    double tie = 0.0;
    double loss = 0.0;

    void normalise() {
        const double total = win + tie + loss;
        win /= total;
        tie /= total;
        loss /= total;
    }
};

}

#endif