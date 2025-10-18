#include <gtest/gtest.h>
#include <algorithm>
#include <iterator>
#include <ostream>

#include "PokerEngine/evaluator/hand_evaluator.hpp"

using PokerEngine::Core::Card;
using namespace PokerEngine::Core::literals;

struct HandEvaluatorParams {
    HandEvaluatorParams(std::vector<Card> community, std::vector<Card> hole, 
        std::vector<Card> expected_best, PokerEngine::Evaluator::HandType expected_classification
    ) : community_cards(std::move(community)), hole_cards(std::move(hole)),
        expected_best_hand(std::move(expected_best)), expected_classification(expected_classification) {}

    std::vector<Card> community_cards;
    std::vector<Card> hole_cards;
    std::vector<Card> expected_best_hand;
    PokerEngine::Evaluator::HandType expected_classification;
};

inline std::ostream& operator<<(std::ostream& os, const HandEvaluatorParams& p) {
    os << "Community: ";
    std::copy(p.community_cards.begin(), p.community_cards.end(), 
        std::ostream_iterator<Card>(os, " ")
    );
    os << " Hole: ";
    std::copy(p.hole_cards.begin(), p.hole_cards.end(),
        std::ostream_iterator<Card>(os, " ")
    );
    
    return os;
}

class HandEvaluatorFixture : public ::testing::TestWithParam<HandEvaluatorParams> {};

TEST_P(HandEvaluatorFixture, TexasHoldEm) {
    auto test_params = GetParam();

    auto texas_hold_em_evaluator = PokerEngine::Evaluator::HandEvaluator();

    std::vector<Card> all_cards = test_params.hole_cards;
    all_cards.insert(all_cards.end(),
                    test_params.community_cards.begin(),
                    test_params.community_cards.end());    
                    
    auto hand_classification = texas_hold_em_evaluator.evaluate(all_cards);
    PokerEngine::Evaluator::HandType hand_type = hand_classification.handtype;
    std::vector<Card> best_hand = hand_classification.besthand;

    ASSERT_EQ(hand_type, test_params.expected_classification);

    //sort for comparison purposes
    std::sort(best_hand.begin(), best_hand.end());
    std::sort(test_params.expected_best_hand.begin(), test_params.expected_best_hand.end());
    ASSERT_EQ(best_hand, test_params.expected_best_hand);
}

INSTANTIATE_TEST_SUITE_P(
    TexasHoldEmHandEvalTests,
    HandEvaluatorFixture,
    ::testing::Values(
        HandEvaluatorParams{
            {"Ac"_c,"9s"_c,"5s"_c,"Kd"_c,"2d"_c},
            {"As"_c,"3h"_c}, 
            {"Ac"_c, "As"_c, "Kd"_c, "9s"_c, "5s"_c}, 
            PokerEngine::Evaluator::HandType::OnePair
        },
        HandEvaluatorParams{
            {"Ac"_c,"9s"_c,"5s"_c,"Ad"_c,"2d"_c},
            {"As"_c,"3h"_c}, 
            {"Ac"_c, "As"_c, "Ad"_c, "5s"_c, "9s"_c}, 
            PokerEngine::Evaluator::HandType::ThreeOfAKind
        },
        HandEvaluatorParams{
            {"Ac"_c,"9s"_c,"5s"_c,"Ad"_c,"3d"_c},
            {"As"_c,"3h"_c}, 
            {"Ac"_c, "As"_c, "Ad"_c, "3d"_c, "3h"_c}, 
            PokerEngine::Evaluator::HandType::FullHouse
        },
        HandEvaluatorParams{
            {"Ac"_c,"Ks"_c,"Ts"_c,"9d"_c,"3d"_c},
            {"Qs"_c,"Jh"_c}, 
            {"Ac"_c, "Ks"_c, "Qs"_c, "Jh"_c, "Ts"_c}, 
            PokerEngine::Evaluator::HandType::Straight
        },
        HandEvaluatorParams{
            {"As"_c,"4h"_c,"Ts"_c,"9d"_c,"5s"_c},
            {"2h"_c,"3s"_c}, 
            {"As"_c, "2h"_c, "3s"_c, "4h"_c, "5s"_c}, 
            PokerEngine::Evaluator::HandType::Straight
        },
        
        HandEvaluatorParams{
            {"Qh"_c,"4h"_c,"Ts"_c,"9d"_c,"Th"_c},
            {"2h"_c,"3h"_c}, 
            {"Qh"_c, "Th"_c, "2h"_c, "3h"_c, "4h"_c}, 
            PokerEngine::Evaluator::HandType::Flush
        },
        HandEvaluatorParams{
            {"Kd"_c,"7h"_c,"2s"_c,"4d"_c,"5h"_c},
            {"Ah"_c,"Qh"_c}, 
            {"Ah"_c, "Kd"_c, "Qh"_c, "7h"_c, "5h"_c}, 
            PokerEngine::Evaluator::HandType::HighCard
        },
        HandEvaluatorParams{
            {"5h"_c,"4h"_c,"Ts"_c,"9d"_c,"6h"_c},
            {"2h"_c,"3h"_c}, 
            {"6h"_c, "5h"_c, "2h"_c, "3h"_c, "4h"_c}, 
            PokerEngine::Evaluator::HandType::StraightFlush
        },
        HandEvaluatorParams{
            {"5h"_c,"5d"_c,"As"_c,"Ad"_c,"4c"_c},
            {"7c"_c,"6c"_c}, 
            {"As"_c, "Ad"_c, "5h"_c, "5d"_c, "7c"_c}, 
            PokerEngine::Evaluator::HandType::TwoPair
        }
    )
);

//TODO: refactor into test suite
TEST(HandEvaluator, HandEvaluatorScores) {
    PokerEngine::Evaluator::HandEvaluator evaluator{};

    std::vector<Card> community = {"Ac"_c, "6h"_c, "7c"_c, "5c"_c, "5s"_c};
    std::vector<Card> hole1 = {"Ad"_c, "5d"_c};//full house
    std::vector<Card> hole2 = {"Kc"_c, "Qc"_c};//flush
    
    std::vector<Card> hand1 = community;
    hand1.insert(hand1.end(), hole1.begin(), hole1.end());

    std::vector<Card> hand2 = community;
    hand2.insert(hand2.end(), hole2.begin(), hole2.end());

    auto hand_eval_1 = evaluator.evaluate(hand1);
    auto hand_eval_2 = evaluator.evaluate(hand2);

    ASSERT_TRUE(hand_eval_1 > hand_eval_2);  
}

TEST(HandEvaluator, HandEvaluatorScores1) {
    PokerEngine::Evaluator::HandEvaluator evaluator{};

    std::vector<Card> community = {"Ac"_c, "6h"_c, "7c"_c, "5c"_c, "5s"_c};
    std::vector<Card> hole1 = {"Kc"_c, "3c"_c};//K-high flush
    std::vector<Card> hole2 = {"8c"_c, "2c"_c};//8-high flush
    
    std::vector<Card> hand1 = community;
    hand1.insert(hand1.end(), hole1.begin(), hole1.end());

    std::vector<Card> hand2 = community;
    hand2.insert(hand2.end(), hole2.begin(), hole2.end());

    auto hand_eval_1 = evaluator.evaluate(hand1);
    auto hand_eval_2 = evaluator.evaluate(hand2);

    ASSERT_TRUE(hand_eval_1 > hand_eval_2);  
}