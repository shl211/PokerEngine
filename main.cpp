#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "PokerEngine/core/card.hpp"
#include "PokerEngine/core/range.hpp"
#include "PokerEngine/core/board.hpp"
#include "PokerEngine/core/hand.hpp"
#include "PokerEngine/core/factory/deck_factory.hpp"
#include "PokerEngine/simulator/poker_simulator.hpp"
#include "PokerEngine/simulator/monte_carlo_strategy.hpp"

using namespace PokerEngine;
using namespace PokerEngine::Core;

std::string getArgValue(int argc, char* argv[], const std::string& flag) {
    for(int i = 1; i < argc - 1; ++i) {
        if(flag == argv[i]) return argv[i+1];
    }
    return {};
}

//TODO: Upgrade to cxxopts.hpp 
int main(int argc, char* argv[]) {
    // Parse flags
    std::string hero_str = getArgValue(argc, argv, "--hero");
    std::string villain_str = getArgValue(argc, argv, "--villain");
    std::string board_str = getArgValue(argc, argv, "--board");
    std::string iter_str = getArgValue(argc, argv, "--iterations");

    if(hero_str.empty() || villain_str.empty() || iter_str.empty()) {
        std::cerr << "Usage: " << argv[0] << " --hero HERO_HAND --villain VILLAIN_HAND --board BOARD --iterations N\n";
        std::cerr << "Example: ./app --hero AsAd --villain KdKh --board Ad --iterations 100000\n";
        return 1;
    }

    int iterations = std::stoi(iter_str);

    // Construct hands
    Hand hero_cards{ { Card(hero_str.substr(0,2)), Card(hero_str.substr(2,2)) } };
    Hand villain_cards{ { Card(villain_str.substr(0,2)), Card(villain_str.substr(2,2)) } };

    // Construct board
    Board board;
    for(size_t i = 0; i < board_str.size(); i += 2) {
        board.add({ Card(board_str.substr(i,2)) });
    }

    // Create ranges
    Range hero_range;
    hero_range.addCombo(hero_cards.get()[0], hero_cards.get()[1]);

    Range villain_range;
    villain_range.addCombo(villain_cards.get()[0], villain_cards.get()[1]);

    // Setup simulator
    auto deck = Factory::DeckFactory::createStandardDeck();
    Simulator::PokerSimulator sim{ hero_range, Board{board}, 1, deck };
    Simulator::MonteCarloNLHStrategy solver{};

    // Run simulation
    auto start = std::chrono::high_resolution_clock::now();
    auto result = sim.simulate(solver, {villain_range}, iterations);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Monte Carlo Results (" << iterations << " iterations)\n";
    std::cout << "Hero win: " << result.win << "\n";
    std::cout << "Villain win: " << result.loss << "\n";
    std::cout << "Tie: " << result.tie << "\n";
    std::cout << "Elapsed: " << elapsed.count() << "s\n";

    return 0;
}