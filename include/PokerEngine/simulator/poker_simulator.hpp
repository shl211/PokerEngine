#ifndef POKER_ENGINE_SIMULATOR_POKER_SIMULATOR_HPP
#define POKER_ENGINE_SIMULATOR_POKER_SIMULATOR_HPP

#include <vector>
#include <iostream>

#include "PokerEngine/core/deck.hpp"
#include "PokerEngine/core/range.hpp"
#include "PokerEngine/evaluator/hand_evaluator.hpp"

namespace PokerEngine::Simulator {
    
struct MonteCarloResult {
    double win = 0.0;
    double tie = 0.0;
    double loss = 0.0;
};

void printCards(const std::vector<Core::Card>& cards) {
    for(auto& c : cards) std::cout << c << " ";
    std::cout << "\n";
}

class PokerSimulator {    
public:
    PokerSimulator(
        const std::vector<Core::Card>& my_hand, 
        const std::vector<Core::Card>& community_cards,
        int num_opponents,
        Core::Deck deck
    ) : my_hand_(my_hand), 
        community_cards_(community_cards), 
        num_opponents_(num_opponents),
        deck_(std::move(deck))
        {}

    MonteCarloResult simulate(
        const std::vector<Core::Range>& opponent_ranges,
        int iterations = 10000
    );
    
    MonteCarloResult simulate(
        const std::vector<Core::Range>& opponent_ranges,
        int iterations,
        unsigned seed
    );

private:
    Core::Deck deck_;
    std::vector<Core::Card> my_hand_;
    std::vector<Core::Card> community_cards_;
    int num_opponents_;
    Evaluator::HandEvaluator eval_{};
};

MonteCarloResult PokerSimulator::simulate(
    const std::vector<Core::Range>& opponent_ranges,
    int iterations,
    unsigned seed
) {
    MonteCarloResult result{};
    std::mt19937 rng(seed);

    if(opponent_ranges.size() != static_cast<size_t>(num_opponents_))
        throw std::invalid_argument("Opponent ranges size does not match num_opponents");

    for(int i = 0; i < iterations; ++i) {
        Core::Deck sim_deck = deck_; // copy deck for this iteration
        sim_deck.shuffle();

        // Remove my hand + community cards from deck
        sim_deck.remove(my_hand_);
        sim_deck.remove(community_cards_);

        // Sample opponent hands
        std::vector<std::vector<Core::Card>> opp_hands;
        for(const auto& range : opponent_ranges) {
            auto combo_opt = range.sample(rng);
            if(!combo_opt.has_value())
                throw std::runtime_error("No available combo for opponent");

            auto combo = *combo_opt;
            opp_hands.push_back({combo.c1, combo.c2});

            sim_deck.remove(combo.c1);
            sim_deck.remove(combo.c2);
        }

        // Complete board if needed
        std::vector<Core::Card> sim_board = community_cards_;
        auto remaining = 5 - community_cards_.size();
        auto drawn = sim_deck.draw(remaining);
        sim_board.insert(sim_board.end(), drawn.begin(), drawn.end());
        // Combine my hand + board
        std::vector<Core::Card> my_cards = my_hand_;
        my_cards.insert(my_cards.end(), sim_board.begin(), sim_board.end());
        auto my_rank = eval_.evaluate(my_cards);
        
        // Evaluate opponents
        std::vector<Evaluator::HandRank> opp_ranks;
        for(auto& h : opp_hands) {
            h.insert(h.end(), sim_board.begin(), sim_board.end());
            opp_ranks.push_back(eval_.evaluate(h));
        }
        
        // Determine best score
        auto best_score = my_rank.score;
        for(auto& r : opp_ranks) if(r.score > best_score) best_score = r.score;

        // Update results
        if(my_rank.score == best_score && 
        std::count_if(opp_ranks.begin(), opp_ranks.end(),
                        [&](auto& r){ return r.score == best_score; }) == 0) {
            result.win += 1.0;
        } else if(my_rank.score == best_score) {
            result.tie += 1.0;
        } else {
            result.loss += 1.0;
        }
    }

    // Normalize probabilities
    result.win /= iterations;
    result.tie /= iterations;
    result.loss /= iterations;

    return result;
}

// Overload without seed
MonteCarloResult PokerSimulator::simulate(
    const std::vector<Core::Range>& opponent_ranges,
    int iterations
) {
    std::random_device rd;
    return simulate(opponent_ranges, iterations, rd());
}


}

#endif