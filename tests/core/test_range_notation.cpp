#include <ostream>
#include <gtest/gtest.h>

#include "PokerEngine/core/range_notation.hpp"

using namespace PokerEngine::Core;
using namespace PokerEngine::Core::literals;

//****************************************************************************//
struct RangeLiteralTestCase {
    RangeToken from_literal;
    RangeToken expected;
};

inline std::ostream& operator<<(std::ostream& os, const RangeLiteralTestCase& r) {
    return os << r.from_literal;
}

class RangeLiteralParser : public ::testing::TestWithParam<RangeLiteralTestCase> {};

TEST_P(RangeLiteralParser, ParsesCorrectly) {
    auto param = GetParam();
    ASSERT_EQ(param.expected, param.from_literal);
}

INSTANTIATE_TEST_SUITE_P(
    AllLiterals,
    RangeLiteralParser,
    ::testing::Values(
        RangeLiteralTestCase{.from_literal = "AA"_r, .expected = {
                                .rank1 = Rank::Ace, .rank2 = Rank::Ace, .type = RangeToken::Type::Pair}},
        RangeLiteralTestCase{.from_literal = "AK"_r, .expected = {
                                .rank1 = Rank::Ace, .rank2 = Rank::King, .type = RangeToken::Type::Normal}},
        RangeLiteralTestCase{.from_literal = "AKs"_r, .expected = {
                                .rank1 = Rank::Ace, .rank2 = Rank::King, .type = RangeToken::Type::Suited}},
        RangeLiteralTestCase{.from_literal = "AKo"_r, .expected = {
                                .rank1 = Rank::Ace, .rank2 = Rank::King, .type = RangeToken::Type::Offsuit}},
        RangeLiteralTestCase{.from_literal = "TT+"_r, .expected = {
                                .rank1 = Rank::Ten, .rank2 = Rank::Ten, .type = RangeToken::Type::Pair, .plus = true}},
        RangeLiteralTestCase{.from_literal = "AQ+"_r, .expected = {
                                .rank1 = Rank::Ace, .rank2 = Rank::Queen, .type = RangeToken::Type::Normal, .plus = true}},
        RangeLiteralTestCase{.from_literal = "AQs+"_r, .expected = {
                                .rank1 = Rank::Ace, .rank2 = Rank::Queen, .type = RangeToken::Type::Suited, .plus = true}},
        RangeLiteralTestCase{.from_literal = "AQo+"_r, .expected = {
                                .rank1 = Rank::Ace, .rank2 = Rank::Queen, .type = RangeToken::Type::Offsuit, .plus = true}}
    )
);

//****************************************************************************//
struct RangeDeducerTestCase {
    RangeToken range;
    std::vector<Hand> rep_hands;
};

inline std::ostream& operator<<(std::ostream& os, const RangeDeducerTestCase& r) {
    return os << r.range;
}

class RangeToCardsDeducer : public ::testing::TestWithParam<RangeDeducerTestCase> {};

TEST_P(RangeToCardsDeducer, DeducesCombosCorrectly) {
    auto [range, represented_hand] = GetParam();
    auto deduced_hands = getHands(range);

    ASSERT_EQ(deduced_hands.size(), represented_hand.size());

    auto deduced_sorted = deduced_hands;
    auto expected_sorted = represented_hand;

    std::sort(deduced_sorted.begin(), deduced_sorted.end());
    std::sort(expected_sorted.begin(), expected_sorted.end());

    ASSERT_EQ(deduced_sorted, expected_sorted);
}

INSTANTIATE_TEST_SUITE_P(
    DeduceRangeRepresentation,
    RangeToCardsDeducer,
    ::testing::Values(
        RangeDeducerTestCase{.range = "AA"_r, .rep_hands = {
            Hand{{"Ad"_c,"Ac"_c}}, Hand{{"Ad"_c,"As"_c}}, Hand{{"Ad"_c,"Ah"_c}},
            Hand{{"Ac"_c,"Ah"_c}}, Hand{{"Ac"_c,"As"_c}}, Hand{{"Ah"_c,"As"_c}}
        }},
        RangeDeducerTestCase{.range = "AK"_r, .rep_hands = {
            Hand{{"Ad"_c, "Kd"_c}}, Hand{{"Ad"_c, "Kh"_c}}, Hand{{"Ad"_c, "Kc"_c}}, Hand{{"Ad"_c, "Ks"_c}},
            Hand{{"Ah"_c, "Kd"_c}}, Hand{{"Ah"_c, "Kh"_c}}, Hand{{"Ah"_c, "Kc"_c}}, Hand{{"Ah"_c, "Ks"_c}},
            Hand{{"Ac"_c, "Kd"_c}}, Hand{{"Ac"_c, "Kh"_c}}, Hand{{"Ac"_c, "Kc"_c}}, Hand{{"Ac"_c, "Ks"_c}},
            Hand{{"As"_c, "Kd"_c}}, Hand{{"As"_c, "Kh"_c}}, Hand{{"As"_c, "Kc"_c}}, Hand{{"As"_c, "Ks"_c}}
        }},
        RangeDeducerTestCase{.range = "AKs"_r, .rep_hands = {
            Hand{{"As"_c, "Ks"_c}}, Hand{{"Ah"_c, "Kh"_c}},
            Hand{{"Ad"_c, "Kd"_c}}, Hand{{"Ac"_c, "Kc"_c}}
        }},
        RangeDeducerTestCase{.range = "AKo"_r, .rep_hands = {
            Hand{{"As"_c, "Kh"_c}}, Hand{{"As"_c, "Kd"_c}}, Hand{{"As"_c, "Kc"_c}},
            Hand{{"Ah"_c, "Ks"_c}}, Hand{{"Ah"_c, "Kd"_c}}, Hand{{"Ah"_c, "Kc"_c}},
            Hand{{"Ad"_c, "Ks"_c}}, Hand{{"Ad"_c, "Kh"_c}}, Hand{{"Ad"_c, "Kc"_c}},
            Hand{{"Ac"_c, "Ks"_c}}, Hand{{"Ac"_c, "Kh"_c}}, Hand{{"Ac"_c, "Kd"_c}}
        }}
    )
);