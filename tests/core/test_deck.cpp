#include <gtest/gtest.h>

#include "PokerEngine/core/deck.hpp"
#include "PokerEngine/core/card.hpp"

using PokerEngine::Core::Deck;
using PokerEngine::Core::Card;
using namespace PokerEngine::Core::literals;

TEST(deck, initialises_correctly) {
    Deck deck{"Ah"_c, "Ad"_c};
    ASSERT_EQ(deck.size(), 2);
    EXPECT_FALSE(deck.empty());
    EXPECT_EQ(deck.view().front(), "Ah"_c);
}

TEST(deck, deck_resets) {
    std::vector<Card> expected{"Ah"_c, "Ac"_c, "Ad"_c, "As"_c};

    Deck deck{expected};
    deck.draw(deck.size());
    ASSERT_TRUE(deck.empty()) << "Deck should be empty after drawing all cards";

    deck.reset();
    ASSERT_EQ(deck.size(), expected.size()) << "Deck should restore to original size after reset";
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(deck.view()[i], expected[i]);
    }
}

TEST(deck, shuffle_with_seed_is_deterministic) {
    using namespace PokerEngine::Core;
    using namespace PokerEngine::Core::literals;

    Deck deck1{"Ah"_c, "Ad"_c, "Ac"_c, "As"_c};
    Deck deck2{"Ah"_c, "Ad"_c, "Ac"_c, "As"_c};

    deck1.shuffle(42);
    deck2.shuffle(42);

    EXPECT_EQ(
        std::vector(deck1.view().begin(), deck1.view().end()),
        std::vector(deck2.view().begin(), deck2.view().end())
    );
}

TEST(deck, iterates_correctly) {
    Deck deck{"Ah"_c, "Ad"_c, "Ac"_c};
    std::vector<Card> iterated;

    for(auto& c : deck) {
        iterated.push_back(c);
    }

    EXPECT_EQ(iterated, 
        std::vector(deck.view().begin(), deck.view().end())
    );
}