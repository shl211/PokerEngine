#include <iostream>
#include "PokerEngine/core/card.hpp"
#include "PokerEngine/core/range.hpp"
#include "PokerEngine/core/factory/deck_factory.hpp"
#include "PokerEngine/simulator/poker_simulator.hpp"

int main() {
    using namespace PokerEngine;
    using namespace PokerEngine::Core::literals;

    auto deck = Core::Factory::DeckFactory::createStandardDeck();
    std::vector<Core::Card> my_hand{"Ts"_c, "Tc"_c};
    std::vector<Core::Card> board{"5d"_c, "6s"_c, "Jh"_c, "4c"_c};


    PokerEngine::Simulator::PokerSimulator sim{
        Core::Hand{my_hand},
        board,
        1,
        deck
    };

    Core::Range opponent_range{};
    opponent_range.addCombo("Ah"_c, "Kh"_c);

    auto res = sim.simulate({opponent_range},100000);

    std::cout << "Monte Carlo Sim Results: \n"
        << "Hero: " << res.win << "\n" 
        << "Villain: " << res.loss << "\n" 
        << "Tie: " << res.tie << "\n";
}
