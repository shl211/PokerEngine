#include <gtest/gtest.h>
#include <algorithm>

#include "PokerEngine/evaluator/detail/bit_mask.hpp"

namespace EvalDetail = PokerEngine::Evaluator::detail;
using EvalDetail::HandMask;
using namespace PokerEngine::Core;
using namespace PokerEngine::Core::literals;
using PokerEngine::Core::operator==;

// Simple helper to count set bits in HandMask
constexpr int count_bits(HandMask mask) noexcept {
    int count = 0;
    while (mask) {
        count += mask & 1;
        mask >>= 1;
    }
    return count;
}

TEST(BitMaskTest, Assumptions) {
    static_assert(static_cast<int>(PokerEngine::Core::Rank::Two)   == 2,  "Rank::Two must be 2");
    static_assert(static_cast<int>(PokerEngine::Core::Rank::Three) == 3,  "Rank::Three must be 3");
    static_assert(static_cast<int>(PokerEngine::Core::Rank::Four)  == 4,  "Rank::Four must be 4");
    static_assert(static_cast<int>(PokerEngine::Core::Rank::Five)  == 5,  "Rank::Five must be 5");
    static_assert(static_cast<int>(PokerEngine::Core::Rank::Six)   == 6,  "Rank::Six must be 6");
    static_assert(static_cast<int>(PokerEngine::Core::Rank::Seven) == 7,  "Rank::Seven must be 7");
    static_assert(static_cast<int>(PokerEngine::Core::Rank::Eight) == 8,  "Rank::Eight must be 8");
    static_assert(static_cast<int>(PokerEngine::Core::Rank::Nine)  == 9,  "Rank::Nine must be 9");
    static_assert(static_cast<int>(PokerEngine::Core::Rank::Ten)   == 10, "Rank::Ten must be 10");
    static_assert(static_cast<int>(PokerEngine::Core::Rank::Jack)  == 11, "Rank::Jack must be 11");
    static_assert(static_cast<int>(PokerEngine::Core::Rank::Queen) == 12, "Rank::Queen must be 12");
    static_assert(static_cast<int>(PokerEngine::Core::Rank::King)  == 13, "Rank::King must be 13");
    static_assert(static_cast<int>(PokerEngine::Core::Rank::Ace)   == 14, "Rank::Ace must be 14");    
}

TEST(BitMaskTest, CardBitmaskSingleCard) {
    Card c{Rank::Ace, Suit::Hearts};
    HandMask mask = EvalDetail::card_bitmask(c);
    EXPECT_EQ(count_bits(mask), 1);
}

TEST(BitMaskTest, CardsBitmaskRoundTrip) {
    std::vector<Card> hand{
        {Rank::Two, Suit::Hearts},
        {Rank::Ace, Suit::Diamonds},
        {Rank::Ten, Suit::Clubs},
        {Rank::Jack, Suit::Spades},
        {Rank::Seven, Suit::Hearts}
    };

    HandMask mask = EvalDetail::cards_bitmask(hand);
    auto reconstructed = EvalDetail::mask_cards(mask);
    EXPECT_EQ(reconstructed.size(), hand.size());

    // Ensure all original cards are present
    for (const auto& c : hand) {
        EXPECT_NE(std::find(reconstructed.begin(), reconstructed.end(), c),
                reconstructed.end());
    }
}

TEST(BitMaskTest, IsStraightNormal) {
    std::vector<Card> hand{"6c"_c, "2h"_c, "3d"_c, "4s"_c, "5d"_c};
    auto mask = EvalDetail::cards_bitmask(hand);
    auto rank_mask = EvalDetail::rank_mask(mask);
    auto straight_res = EvalDetail::is_straight(rank_mask);

    ASSERT_TRUE(straight_res.is_straight);
    ASSERT_EQ(straight_res.high_card, Rank::Six);
}

TEST(BitMaskTest, IsStraightWheel) {
    std::vector<Card> hand{"Ac"_c, "2h"_c, "3d"_c, "4s"_c, "5d"_c};
    auto mask = EvalDetail::cards_bitmask(hand);
    auto rank_mask = EvalDetail::rank_mask(mask);
    auto straight_res = EvalDetail::is_straight(rank_mask);
    
    ASSERT_TRUE(straight_res.is_straight);
    ASSERT_EQ(straight_res.high_card, Rank::Five);
}

TEST(BitMaskTest, IsNotStraight) {
    std::vector<Card> hand{"2h"_c, "3d"_c, "4s"_c, "5d"_c,  "Qd"_c};
    auto mask = EvalDetail::cards_bitmask(hand);
    auto rank_mask = EvalDetail::rank_mask(mask);
    auto straight_res = EvalDetail::is_straight(rank_mask);

    ASSERT_FALSE(straight_res.is_straight);
}

TEST(BitMaskTest, CountRank) {
    std::vector<Card> hand{"2h"_c, "2d"_c, "Td"_c, "Qh"_c, "Qd"_c};
    auto mask = EvalDetail::cards_bitmask(hand);
    auto count_res = EvalDetail::count_ranks(mask);

    std::array<int, 13> expected {2,0,0,0,0,0,0,0,1,0,2,0,0};

    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(count_res[i], expected[i]) << "Mismatch at rank index " << i;
    }
}