#ifndef POKER_ENGINE_SIMULATOR_POKER_SIMULATOR_HPP
#define POKER_ENGINE_SIMULATOR_POKER_SIMULATOR_HPP

#include <vector>
#include <iostream>

#include "PokerEngine/core/deck.hpp"
#include "PokerEngine/core/range.hpp"
#include "PokerEngine/core/hand.hpp"
#include "PokerEngine/core/board.hpp"
#include "PokerEngine/evaluator/hand_evaluator.hpp"
#include "PokerEngine/simulator/monte_carlo_strategy.hpp"

namespace PokerEngine::Simulator {

template<typename SimStrategy>
concept PokerSimStrategy = requires(const SimStrategy& s,
                                 const Core::Range& my_range,
                                 const Core::Board& board,
                                 int num_opponents,
                                 Core::Deck deck,
                                 const std::vector<Core::Range>& opponent_ranges,
                                 int iterations,
                                 unsigned seed) {
    { s.run(my_range, board, num_opponents, deck, opponent_ranges, iterations, seed) }
        -> std::same_as<SimResult>;
};


class PokerSimulator {
public:
    PokerSimulator(Core::Range my_range, Core::Board board, int num_opponents, Core::Deck deck)
        : my_range_(std::move(my_range)), community_cards_(std::move(board)),
          num_opponents_(num_opponents), deck_(std::move(deck)) {}

    template<typename SimStrategy>
    SimResult simulate(const SimStrategy& strategy,
                            const std::vector<Core::Range>& opponent_ranges,
                            int iterations,
                            unsigned seed = std::random_device{}()) const
    {
        return strategy.run(my_range_, community_cards_, num_opponents_, deck_, opponent_ranges, iterations, seed);
    }

private:
    Core::Range my_range_;
    Core::Board community_cards_;
    int num_opponents_;
    Core::Deck deck_;
};
}

#endif