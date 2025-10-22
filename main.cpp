#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>

#include <cxxopts.hpp>

#include "PokerEngine/core/card.hpp"
#include "PokerEngine/core/range.hpp"
#include "PokerEngine/core/board.hpp"
#include "PokerEngine/core/hand.hpp"
#include "PokerEngine/core/factory/deck_factory.hpp"
#include "PokerEngine/simulator/poker_simulator.hpp"
#include "PokerEngine/simulator/monte_carlo_strategy.hpp"

using namespace PokerEngine;
using namespace PokerEngine::Core;

struct SimulationStats {
    double win;
    double loss;
    double tie;
    double elapsed;
};

SimulationStats run_simulation(const std::string& hero_str,
                                const std::string& villain_str,
                                const std::string& board_str,
                                int iterations)
{
    Hand hero_cards{{ Card(hero_str.substr(0,2)), Card(hero_str.substr(2,2)) }};
    Hand villain_cards{{ Card(villain_str.substr(0,2)), Card(villain_str.substr(2,2)) }};

    Board board;
    for (size_t i = 0; i < board_str.size(); i += 2)
        board.add({ Card(board_str.substr(i,2)) });

    Range hero_range;
    hero_range.addCombo(hero_cards.get()[0], hero_cards.get()[1]);
    Range villain_range;
    villain_range.addCombo(villain_cards.get()[0], villain_cards.get()[1]);

    auto deck = Factory::DeckFactory::createStandardDeck();
    Simulator::PokerSimulator sim{ hero_range, Board{board}, 1, deck };
    Simulator::MonteCarloNLHStrategy solver{};

    auto start = std::chrono::high_resolution_clock::now();
    auto result = sim.simulate(solver, {villain_range}, iterations);
    auto end = std::chrono::high_resolution_clock::now();

    double elapsed = std::chrono::duration<double>(end - start).count();

    return { result.win, result.loss, result.tie, elapsed };
}

void printUsageInfo(const std::string& program_name) {
    std::cerr << "Usage: " << program_name << " --hero HERO_HAND --villain VILLAIN_HAND --board BOARD --iterations N\n";
    std::cerr << "Example: ./app --hero AsAd --villain KdKh --board Ad --iterations 100000\n";
}

int main(int argc, char* argv[]) {
    cxxopts::Options options("PokerSolver", "Equity calculation for Texas No-Limit Hold'em");

    options.add_options()
        ("hero", "Hero cards", cxxopts::value<std::string>())
        ("villain", "Villain cards", cxxopts::value<std::string>())
        ("board", "Cards on board", cxxopts::value<std::string>()->default_value(""))
        ("iterations", "Iterations for simulation", cxxopts::value<int>()->default_value("10000"))
        ("h,help", "Print usage");

    auto args = options.parse(argc, argv);

    if (args.count("help"))
    {
        std::cout << options.help() << std::endl;
        printUsageInfo(argv[0]);
        exit(0);
    }

    if (!args.count("hero") || !args.count("villain")) {
        std::cerr << "Error: Both --hero and --villain must be provided.\n\n"
                    << options.help() << std::endl;
        printUsageInfo(argv[0]);
        return 1;
    }

    std::string hero_str = args["hero"].as<std::string>();
    std::string villain_str = args["villain"].as<std::string>();
    std::string board_str = args["board"].as<std::string>();

    int iterations = args["iterations"].as<int>();

    try {
        auto stats = run_simulation(hero_str, villain_str, board_str, iterations);

        std::cout << std::fixed << std::setprecision(4);
        std::cout << "=== Monte Carlo Simulation ===\n";
        std::cout << "Iterations:  " << iterations << "\n";
        std::cout << "Hero win:    " << stats.win * 100 << "%\n";
        std::cout << "Villain win: " << stats.loss * 100 << "%\n";
        std::cout << "Tie:         " << stats.tie * 100 << "%\n";
        std::cout << "Elapsed:     " << stats.elapsed << " s\n";
    } catch (const std::exception& e) {
        std::cerr << "Simulation error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}