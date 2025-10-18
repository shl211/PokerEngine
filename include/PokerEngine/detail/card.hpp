#ifndef POKER_ENGINE_CORE_DETAIL_CARD_HPP
#define POKER_ENGINE_CORE_DETAIL_CARD_HPP

#include <stdexcept>

namespace PokerEngine::Core::detail {
    enum class Rank;
    enum class Suit;

    constexpr inline Rank char_to_rank(char c) {
    switch (c) {
        case '2': return Rank::Two;
        case '3': return Rank::Three;
        case '4': return Rank::Four;
        case '5': return Rank::Five;
        case '6': return Rank::Six;
        case '7': return Rank::Seven;
        case '8': return Rank::Eight;
        case '9': return Rank::Nine;
        case 'T': return Rank::Ten;
        case 'J': return Rank::Jack;
        case 'Q': return Rank::Queen;
        case 'K': return Rank::King;
        case 'A': return Rank::Ace;
        default: throw std::invalid_argument("Invalid rank character");
    }
}

constexpr inline Suit char_to_suit(char c) {
    switch (c) {
        case 'h': return Suit::Hearts;
        case 'd': return Suit::Diamonds;
        case 'c': return Suit::Clubs;
        case 's': return Suit::Spades;
        default: throw std::invalid_argument("Invalid suit character");
    }
}

inline std::string to_string(const Rank& r) {
    switch (r) {
        case Rank::Two:
            return "2";
        case Rank::Three:
            return "3";
        case Rank::Four:
            return "4";
        case Rank::Five:
            return "5";
        case Rank::Six:
            return "6";
        case Rank::Seven:
            return "7";
        case Rank::Eight:
            return "8";
        case Rank::Nine:
            return "9";
        case Rank::Ten:
            return "T";
        case Rank::Jack:
            return "J";
        case Rank::Queen:
            return "Q";
        case Rank::King:
            return "K";
        case Rank::Ace:
            return "A";
    }

    return "Unkown Rank";
}

inline std::string to_string(const Suit& s) {
    switch (s) {
        case Suit::Hearts:
            return "h";
        case Suit::Spades:
            return "s";
        case Suit::Clubs:
            return "c";
        case Suit::Diamonds:
            return "d";
    }

    return "Unknown Suit";
}

}

#endif