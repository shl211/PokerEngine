#include <iostream>
#include <chrono>
#include "PokerEngine/core/card.hpp"
#include "PokerEngine/core/range.hpp"
#include "PokerEngine/core/factory/deck_factory.hpp"
#include "PokerEngine/simulator/poker_simulator.hpp"
#include "PokerEngine/simulator/monte_carlo_strategy.hpp"

int main() {
    using namespace PokerEngine;
    using namespace PokerEngine::Core::literals;
    using namespace std::chrono;

    auto start = high_resolution_clock::now();

    auto deck = Core::Factory::DeckFactory::createStandardDeck();
    std::vector<Core::Card> my_hand{"Ts"_c, "Tc"_c};
    std::vector<Core::Card> board{"5d"_c, "6s"_c, "Jh"_c, "4c"_c};

    Core::Range my_range{};
    my_range.addCombo(my_hand[0],my_hand[1],1.0);

    PokerEngine::Simulator::PokerSimulator sim{
        my_range,
        Core::Board{board},
        1,
        deck
    };

    Core::Range opponent_range{};
    opponent_range.addCombo("Ah"_c, "Kh"_c);

    int iterations = 100000;
    Simulator::MonteCarloNLHStrategy solver{};
    auto res = sim.simulate(solver, {opponent_range}, iterations);

    auto end = high_resolution_clock::now();  // end timer
    duration<double> elapsed = end - start;

    std::cout << "Monte Carlo Sim Results (" << iterations << " iterations):\n"
            << "Hero win: " << res.win << "\n"
            << "Villain win: " << res.loss << "\n"
            << "Tie: " << res.tie << "\n"
            << "Elapsed: " << elapsed.count() << "s\n";
}
