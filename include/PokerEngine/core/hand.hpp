#ifndef POKER_ENGINE_CORE_HAND_HPP
#define POKER_ENGINE_CORE_HAND_HPP

#include <vector>

#include "PokerEngine/core/card.hpp"

namespace PokerEngine::Core {

class Hand {
public:

    Hand() = default;
    explicit Hand(std::vector<Core::Card> cards) : hand_(std::move(cards)) {}

    size_t size() { return hand_.size(); }    

    void add(Card c) { hand_.push_back(c); }
    void add(const std::vector<Card>& cards) { hand_.insert(hand_.end(), cards.begin(), cards.end()); }
    void pop(Card c) { hand_.pop_back(); }
    void clear() { hand_.clear(); }

    const std::vector<Card>& get() { return hand_; };
    const std::vector<Card>& get() const { return hand_; };

    auto begin() noexcept { return hand_.begin(); }
    auto end() noexcept { return hand_.end(); }
    auto begin() const noexcept { return hand_.cbegin(); }
    auto end() const noexcept { return hand_.cend(); }

private:
    std::vector<Card> hand_; 
};

inline bool operator<(const Hand& lhs, const Hand& rhs) {
    auto l = lhs.get();
    auto r = rhs.get();
    std::sort(l.begin(), l.end());
    std::sort(r.begin(), r.end());
    return l < r; // lexicographical compare
}

inline bool operator==(const Hand& lhs, const Hand& rhs) {
    auto l = lhs.get();
    auto r = rhs.get();
    std::sort(l.begin(), l.end());
    std::sort(r.begin(), r.end());
    return l == r;
}

}


#endif