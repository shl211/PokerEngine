#ifndef POKER_ENGINE_CORE_DECK_HPP
#define POKER_ENGINE_CORE_DECK_HPP

#include <vector>
#include <stdexcept>
#include <span>
#include <random>
#include <ranges>
#include <algorithm>

#include "PokerEngine/core/card.hpp"

namespace PokerEngine::Core {

class Deck {
public:
    explicit Deck(std::vector<Card> cards) 
        : cards_(cards), original_cards_{cards} {}

    Deck(std::initializer_list<Card> cards) 
        : cards_(cards), original_cards_(cards) {}

    void shuffle();
    void shuffle(unsigned seed);
    void reset() noexcept { cards_ = original_cards_; }
    
    Card draw();
    std::vector<Card> draw(size_t n);

    size_t size() const noexcept { return cards_.size(); }
    bool empty() const noexcept { return cards_.empty(); }
    
    auto begin() noexcept { return cards_.begin(); }
    auto end() noexcept { return cards_.end(); }
    auto begin() const noexcept { return cards_.cbegin(); }
    auto end() const noexcept { return cards_.cend(); }

    std::span<const Card> view() const noexcept { return cards_; }

private:
    std::vector<Card> cards_;
    std::vector<Card> original_cards_;
};

void Deck::shuffle() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::ranges::shuffle(cards_, gen);
}

void Deck::shuffle(unsigned seed) {
    std::mt19937 gen(seed);
    std::ranges::shuffle(cards_, gen);
}

Card Deck::draw() {
    if (cards_.empty()) {
        throw std::out_of_range("Cannot draw from empty deck");
    }

    auto selected = cards_.back();
    cards_.pop_back();
    return selected;
}

std::vector<Card> Deck::draw(size_t n) {
    if (n > cards_.size()) {
        throw std::out_of_range("Cannot draw more cards than are in the deck");
    }

    auto start = cards_.end() - n;
    std::vector<Card> drawn(start, cards_.end());
    cards_.erase(start, cards_.end());
    return drawn;
}

}

#endif