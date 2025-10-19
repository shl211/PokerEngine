#ifndef POKER_ENGINE_CORE_FACTORY_DECK_FACTORY_HPP
#define POKER_ENGINE_CORE_FACTORY_DECK_FACTORY_HPP

#include "PokerEngine/core/card.hpp"
#include "PokerEngine/core/deck.hpp"

namespace PokerEngine::Core::Factory {

class DeckFactory {
public:
    DeckFactory() = default;

    [[nodiscard]] static Deck createStandardDeck();
    [[nodiscard]] static Deck createShortDeck();
};

namespace {
    constexpr int STANDARD_DECK_SIZE = 52;
    constexpr int SHORT_DECK_SIZE = 36;
}

Deck DeckFactory::createStandardDeck() {
    std::vector<Card> cards;
    cards.reserve(STANDARD_DECK_SIZE);
    
    for (Suit s : {Suit::Hearts, Suit::Diamonds, Suit::Clubs, Suit::Spades}) {
        for (Rank r : {Rank::Two, Rank::Three, Rank::Four, Rank::Five, Rank::Six, Rank::Seven,
                    Rank::Eight, Rank::Nine, Rank::Ten, Rank::Jack, Rank::Queen, Rank::King, Rank::Ace}) {
            cards.emplace_back(r, s);
        }
    }

    return Deck{std::move(cards)};
}

Deck DeckFactory::createShortDeck() {
    std::vector<Card> cards;
    cards.reserve(SHORT_DECK_SIZE);
    
    for (Suit s : {Suit::Hearts, Suit::Diamonds, Suit::Clubs, Suit::Spades}) {
        for (Rank r : {Rank::Two, Rank::Three, Rank::Four, Rank::Five, Rank::Six, Rank::Seven,
                    Rank::Eight, Rank::Nine, Rank::Ten, Rank::Jack, Rank::Queen, Rank::King, Rank::Ace}) {
            cards.emplace_back(r, s);
        }
    }

    return Deck{std::move(cards)};
}

}

#endif