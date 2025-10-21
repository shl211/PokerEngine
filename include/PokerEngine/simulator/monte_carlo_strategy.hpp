#ifndef POKER_ENGINE_SIMULATOR_MONTE_CARLO_STRATEGY_HPP
#define POKER_ENGINE_SIMULATOR_MONTE_CARLO_STRATEGY_HPP

#include <vector>
#include <random>

#include "PokerEngine/core/card.hpp"
#include "PokerEngine/core/hand.hpp"
#include "PokerEngine/core/range.hpp"
#include "PokerEngine/core/board.hpp"
#include "PokerEngine/core/deck.hpp"
#include "PokerEngine/evaluator/hand_evaluator.hpp"
#include "PokerEngine/simulator/sim_result.hpp"

namespace PokerEngine::Simulator {

class MonteCarloNLHStrategy {
public:
    MonteCarloNLHStrategy() = default;

    SimResult run(
        const Core::Range& my_range,
        const Core::Board& community,
        int num_opponents,
        Core::Deck deck, 
        const std::vector<Core::Range>& opponent_ranges,
        int iterations,
        unsigned seed = std::random_device{}()
    ) const;

private:
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

    Core::Hand sampleHandFromRange(Core::Range range,
                                Core::Deck& deck, std::mt19937& rng) {
        auto combo_opt = range.sample(rng);
        if(!combo_opt.has_value())
            throw std::runtime_error("No available combo for opponent");

        auto combo = *combo_opt;
        deck.remove({combo.c1, combo.c2});
        return Core::Hand{{combo.c1, combo.c2}};
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

    SimResult MonteCarloNLHStrategy::run (
        const Core::Range& my_range,
        const Core::Board& community,
        int num_opponents,
        Core::Deck deck, 
        const std::vector<Core::Range>& opponent_ranges,
        int iterations,
        unsigned seed
    ) const
    {
        SimResult result{};
        std::mt19937 rng(seed);
        deck.remove(community.get());

        if(opponent_ranges.size() != static_cast<size_t>(num_opponents))
            throw std::invalid_argument("Opponent ranges size does not match num_opponents");

        for (int i = 0; i < iterations; ++i) {
            Core::Deck sim_deck = deck; // copy deck
            sim_deck.shuffle();

            Core::Hand hero_hand = sampleHandFromRange(my_range,deck,rng);

            // Copy opponent ranges and remove blocked cards
            std::vector<Core::Range> opp_ranges = opponent_ranges;
            for (auto& r : opp_ranges)
                r.removeBlocked(hero_hand.get());

            std::vector<Core::Hand> opp_hands;
            opp_hands.reserve(num_opponents);

            for (int i = 0; i < num_opponents; ++i) {
                auto& r = opp_ranges[i];

                auto combo_opt = r.sample(rng);
                if (!combo_opt) throw std::runtime_error("No available combo for opponent");
                auto combo = *combo_opt;
                Core::Hand hand{{combo.c1, combo.c2}};
                opp_hands.push_back(hand);

                sim_deck.remove(combo.c1);
                sim_deck.remove(combo.c2);

                // Remove these blockers from future players 
                for (int j = i + 1; j < num_opponents; ++j) {
                    opp_ranges[j].removeBlocked({combo.c1,combo.c2});
                }
            }

            auto sim_board = completeBoard(community.get(), MAX_BOARD_SIZE_NLH, sim_deck);
            auto hero_cards = combineCards(hero_hand.get(), sim_board.get());
            auto hero_rank = eval_.evaluate(hero_cards);

            // Evaluate opponents
            std::vector<Evaluator::HandRank> opp_ranks;
            for (auto& h : opp_hands) {
                h.add(sim_board.get());
                opp_ranks.push_back(eval_.evaluate(h.get()));
            }

            // Determine best score & update results
            auto best_score = hero_rank.score;
            for (auto& r : opp_ranks)
                if (r.score > best_score) best_score = r.score;

            if (hero_rank.score == best_score &&
                std::count_if(opp_ranks.begin(), opp_ranks.end(),
                            [&](auto& r){ return r.score == best_score; }) == 0) {
                result.win += 1.0;
            } else if (hero_rank.score == best_score) {
                result.tie += 1.0;
            } else {
                result.loss += 1.0;
            }
        }

        result.normalise();
        return result;
    }
}

#endif