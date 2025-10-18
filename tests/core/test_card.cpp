#include <gtest/gtest.h>
#include <ostream>

#include "PokerEngine/core/card.hpp"

using PokerEngine::Core::Rank;
using PokerEngine::Core::Suit;
using PokerEngine::Core::Card;

struct CardStringParserParams {
    CardStringParserParams(
        const std::string& card_str,
        Rank rank,
        Suit suit
    ) : card_str(card_str), defined_suit(suit), defined_rank(rank) {}

    std::string card_str;
    Suit defined_suit;
    Rank defined_rank;
};

inline std::ostream& operator<<(std::ostream& os, const CardStringParserParams& p) {
    return os << p.card_str;
}

class CardStringParser : public ::testing::TestWithParam<CardStringParserParams> {};

TEST_P(CardStringParser, Parsing) {
    auto [card_str, defined_suit, defined_rank] = GetParam();

    Card card{ card_str };

    ASSERT_EQ(card.rank(), defined_rank);
    ASSERT_EQ(card.suit(), defined_suit);
}

INSTANTIATE_TEST_SUITE_P(
    CardStringParsingValid,
    CardStringParser,
    ::testing::Values(
        CardStringParserParams("Kh", Rank::King, Suit::Hearts),
        CardStringParserParams("As", Rank::Ace, Suit::Spades),
        CardStringParserParams("Td", Rank::Ten, Suit::Diamonds),
        CardStringParserParams("2c", Rank::Two, Suit::Clubs)
    )
);