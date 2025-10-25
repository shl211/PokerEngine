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

SimulationStats run_simulation(const Range& hero_range,
                                const Range& villain_range,
                                const Board& board,
                                int iterations)
{
    auto deck = Factory::DeckFactory::createStandardDeck();
    Simulator::PokerSimulator sim{ hero_range, Board{board}, 1, deck };
    Simulator::MonteCarloNLHStrategy solver{};

    auto start = std::chrono::high_resolution_clock::now();
    auto result = sim.simulate(solver, {villain_range}, iterations);
    auto end = std::chrono::high_resolution_clock::now();

    double elapsed = std::chrono::duration<double>(end - start).count();

    return { result.win, result.loss, result.tie, elapsed };
}

Hand parse_hand(const std::string& s) {
    if (s.empty()) return {}; // empty hand
    Hand h;
    if (s.size() % 2 != 0) throw std::invalid_argument("Invalid hand string");
    for (size_t i = 0; i < s.size(); i += 2) {
        h.add(Card(s.substr(i, 2)));
    }
    return h;
}

Board parse_board(const std::string& s) {
    if (s.empty()) return {}; // empty board
    Board h;
    if (s.size() % 2 != 0) throw std::invalid_argument("Invalid board string");
    for (size_t i = 0; i < s.size(); i += 2) {
        h.add(Card(s.substr(i, 2)));
    }
    return h;
}

Range parse_range(const std::string& s) {
    Range range;
    if (s.empty()) return range;

    std::stringstream ss(s);
    std::string token_str;
    while (std::getline(ss, token_str, ',')) {
        token_str.erase(std::remove_if(token_str.begin(), token_str.end(), ::isspace), token_str.end());
        if (!token_str.empty()) {
            range.addCombo(from_string(token_str));
        }
    }
    return range;
}

void printUsageInfo(const std::string& program_name) {
    std::cerr << "Usage: " << program_name << " [--hero HERO_HAND | --hero-range HERO_RANGE] [--villain VILLAIN_HAND | --villain-range VILLAIN_RANGE] --board BOARD --iterations N\n";
    std::cerr << "Example (single hands): ./app --hero AsAd --villain KdKh --board Ad --iterations 100000\n";
    std::cerr << "Example (ranges): ./app --hero-range \"AsAd,AcAh\" --villain-range \"KdKh,KcKs\" --board Ad --iterations 100000\n";
}

int main(int argc, char* argv[]) {
    cxxopts::Options options("PokerSolver", "Equity calculation for Texas No-Limit Hold'em");

    options.add_options()
        ("hero", "Hero cards", cxxopts::value<std::string>())
        ("villain", "Villain cards", cxxopts::value<std::string>())
        ("board", "Cards on board", cxxopts::value<std::string>()->default_value(""))
        ("hero-range", "Hero range e.g. TT,AT+", cxxopts::value<std::string>())
        ("villain-range", "Villain range e.g. TT,AT+", cxxopts::value<std::string>())
        ("iterations", "Iterations for simulation", cxxopts::value<int>()->default_value("10000"))
        ("h,help", "Print usage");

    auto args = options.parse(argc, argv);

    if (args.count("help"))
    {
        std::cout << options.help() << std::endl;
        printUsageInfo(argv[0]);
        exit(0);
    }

    bool hero_defined_once = args.count("hero") ^ args.count("hero-range");
    bool villain_defined_once = args.count("villain") ^ args.count("villain-range");

    if (!hero_defined_once || !villain_defined_once) {
        std::cerr << "Error: Either --hero or --hero-range must be provided, "
                    << "and either --villain or --villain-range must be provided.\n\n"
                    << options.help() << std::endl;
        printUsageInfo(argv[0]);
        return 1;
    }

    Range hero_range{};
    Range villain_range{};

    if(args.count("hero")) {
        Hand hero_cards = parse_hand(args["hero"].as<std::string>());
        hero_range.addCombo(hero_cards.get()[0], hero_cards.get()[1]);
    } else {
        hero_range = parse_range(args["hero-range"].as<std::string>());
    }

    if(args.count("villain")) {
        Hand villain_cards = parse_hand(args["villain"].as<std::string>());
        villain_range.addCombo(villain_cards.get()[0], villain_cards.get()[1]);
    } else {
        villain_range = parse_range(args["villain-range"].as<std::string>());
    }

    Board board = parse_board(args["board"].as<std::string>());
    int iterations = args["iterations"].as<int>();

    try {
        auto stats = run_simulation(hero_range, villain_range, board, iterations);

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