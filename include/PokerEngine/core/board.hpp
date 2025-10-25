#ifndef POKER_ENGINE_CORE_BOARD_HPP
#define POKER_ENGINE_CORE_BOARD_HPP

#include <vector>

#include "PokerEngine/core/card.hpp"

namespace PokerEngine::Core {

class Board {
public:
    Board() = default;
    explicit Board(std::vector<Core::Card> cards) : cards_(std::move(cards)) {}
    
    const std::vector<Core::Card>& cards() const { return cards_; }
    void add(const Core::Card& c) { cards_.push_back(c); }
    void add(const std::vector<Card>& c) { cards_.insert(cards_.end(), c.begin(), c.end()); }
    size_t size() const { return cards_.size(); }

    const std::vector<Card>& get() { return cards_; }
    const std::vector<Card>& get() const { return cards_; }

private:
    std::vector<Core::Card> cards_;
};

}

#endif