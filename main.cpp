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

SimulationStats run_simulation(const Hand& hero_str,
                                const Hand& villain_str,
                                const Board& board_str,
                                int iterations)
{
    Hand hero_cards{hero_str};
    Hand villain_cards{villain_str};
    Board board{board_str};

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

PokerEngine::Core::Hand parse_hand(const std::string& s) {
    if (s.empty()) return {}; // empty hand
    PokerEngine::Core::Hand h;
    if (s.size() % 2 != 0) throw std::invalid_argument("Invalid hand string");
    for (size_t i = 0; i < s.size(); i += 2) {
        h.add(PokerEngine::Core::Card(s.substr(i, 2)));
    }
    return h;
}

PokerEngine::Core::Board parse_board(const std::string& s) {
    if (s.empty()) return {}; // empty board
    PokerEngine::Core::Board h;
    if (s.size() % 2 != 0) throw std::invalid_argument("Invalid hand string");
    for (size_t i = 0; i < s.size(); i += 2) {
        h.add(PokerEngine::Core::Card(s.substr(i, 2)));
    }
    return h;
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

    Hand hero_str = parse_hand(args["hero"].as<std::string>());
    Hand villain_str = parse_hand(args["villain"].as<std::string>());
    Board board_str = parse_board(args["board"].as<std::string>());

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