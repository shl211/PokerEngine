#ifndef POKER_ENGINE_SIMULATOR_POKER_SIMULATOR_HPP
#define POKER_ENGINE_SIMULATOR_POKER_SIMULATOR_HPP

#include <vector>
#include <iostream>

#include "PokerEngine/core/deck.hpp"
#include "PokerEngine/core/range.hpp"
#include "PokerEngine/core/hand.hpp"
#include "PokerEngine/core/board.hpp"
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
        const Core::Hand& my_hand, 
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
    Core::Hand my_hand_;
    std::vector<Core::Card> community_cards_;
    int num_opponents_;
    Evaluator::HandEvaluator eval_{};
};

namespace {
    constexpr int MAX_BOARD_SIZE_NLH = 5;

    std::vector<Core::Hand> sampleCardsFromRange(int num_players, std::vector<Core::Range> ranges,
                                                    Core::Deck& deck, std::mt19937& rng) 
    {
        std::vector<Core::Hand> opp_hands;
        opp_hands.reserve(num_players);
        for(const auto& range : ranges) {
            auto combo_opt = range.sample(rng);
            if(!combo_opt.has_value())
                throw std::runtime_error("No available combo for opponent");

            auto combo = *combo_opt;
            Core::Hand hand{{combo.c1, combo.c2}};
            opp_hands.push_back(std::move(hand));

            deck.remove(combo.c1);
            deck.remove(combo.c2);
        }

        return opp_hands;
    }
    
    Core::Board completeBoard(const std::vector<Core::Card>& curr_board, int total_cards_on_board,
                                            Core::Deck& deck) 
    {
        Core::Board sim_board{curr_board};
        auto remaining = total_cards_on_board - curr_board.size();
        auto drawn = deck.draw(remaining);
        sim_board.add(std::move(drawn));
        return sim_board;
    }

    std::vector<Core::Card> combineCards(const std::vector<Core::Card>& lhs, const std::vector <Core::Card>& rhs) {
        std::vector<Core::Card> comb = lhs;
        comb.insert(comb.end(), rhs.begin(), rhs.end());
        return comb;
    }
}

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
        sim_deck.remove(my_hand_.get());
        sim_deck.remove(community_cards_);

        std::vector<Core::Hand> opp_hands = sampleCardsFromRange(num_opponents_, opponent_ranges, sim_deck, rng);
        Core::Board sim_board = completeBoard(community_cards_,MAX_BOARD_SIZE_NLH,sim_deck);
        std::vector<Core::Card> my_cards = combineCards(my_hand_.get(), sim_board.get());
        auto my_rank = eval_.evaluate(my_cards);

        // Evaluate opponents
        std::vector<Evaluator::HandRank> opp_ranks;
        for(auto& h : opp_hands) {
            h.add(sim_board.get());
            opp_ranks.push_back(eval_.evaluate(h.get()));
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

MonteCarloResult PokerSimulator::simulate(
    const std::vector<Core::Range>& opponent_ranges,
    int iterations
) {
    std::random_device rd;
    return simulate(opponent_ranges, iterations, rd());
}


}

#endif