#ifndef POKER_ENGINE_CORE_CARD_HPP
#define POKER_ENGINE_CORE_CARD_HPP

#include <string>
#include <ostream>

#include "PokerEngine/core/card_enum.hpp"
#include "PokerEngine/core/detail/card_ops.hpp"

namespace PokerEngine::Core {

class Card {

public:
    constexpr Card() noexcept : rank_(Rank::Ace), suit_(Suit::Spades) {}
    constexpr Card(Rank r, Suit s) noexcept : rank_(r), suit_(s) {};
    constexpr explicit Card(std::string_view s) {
        if(s.length() != 2) throw std::invalid_argument("Invalid card string, expecting standard notation e.g. Kh (King of Hearts)");
        rank_ = detail::char_to_rank(s[0]);
        suit_ = detail::char_to_suit(s[1]);
    }

    constexpr Suit suit() const noexcept { return suit_; }
    constexpr Rank rank() const noexcept { return rank_; }

private:
    Suit suit_;
    Rank rank_;

};

constexpr inline bool operator==(const Card& lhs, const Card& rhs) noexcept {
    return lhs.rank() == rhs.rank() && lhs.suit() == rhs.suit();
}

constexpr inline bool operator!=(const Card& lhs, const Card& rhs) noexcept {
    return !(lhs == rhs);
}

constexpr inline auto operator<=>(const Card& lhs, const Card& rhs) noexcept {
    if (auto cmp = lhs.rank() <=> rhs.rank(); cmp != 0)
        return cmp;                       // primary comparison by rank
    return lhs.suit() <=> rhs.suit();     // tie-break by suit
}

inline std::string to_string(const Card& c) {
    return detail::to_string(c.rank()) + "" + detail::to_string(c.suit());
}

inline std::ostream& operator<<(std::ostream& os, const Card& c) {
    return os << to_string(c);
}

namespace literals {
    // Creates a Card from a string literal, e.g. "Ah"_c
    constexpr Card operator "" _c(const char* s, size_t) {
        return Card{s};
    }
}
}


#endif