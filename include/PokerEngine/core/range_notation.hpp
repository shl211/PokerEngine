#ifndef POKER_ENGINE_CORE_RANGE_NOTATION_HPP
#define POKER_ENGINE_CORE_RANGE_NOTATION_HPP

#include <cstdint>
#include <vector>
#include <ostream>

#include "PokerEngine/core/hand.hpp"
#include "PokerEngine/core/card.hpp"
#include "PokerEngine/core/card_enum.hpp"
#include "PokerEngine/core/detail/card_ops.hpp"

namespace PokerEngine::Core {

struct RangeToken {
    enum class Type { Normal, Pair, Suited, Offsuit };
    Rank rank1;
    Rank rank2;
    Type type;
};

namespace {
    inline std::string to_string(const RangeToken::Type& type) {
        switch(type) {
            case RangeToken::Type::Suited: return "s";
            case RangeToken::Type::Offsuit: return "o";   
        }
        return "";
    }
}

inline std::ostream& operator<<(std::ostream& os, const RangeToken& token) {
    return os << detail::to_string(token.rank1) << detail::to_string(token.rank2)
        << to_string(token.type);
} 

constexpr inline bool operator==(const RangeToken& lhs, const RangeToken& rhs) {
    return lhs.rank1 == rhs.rank1 &&
            lhs.rank2 == rhs.rank2 &&
            lhs.type == rhs.type;
}

namespace detail {
    inline std::vector<Hand> getSuitedCombos(Rank r1, Rank r2) {
        std::vector<Hand> combos;
        for (Suit s : {Suit::Hearts, Suit::Diamonds, Suit::Clubs, Suit::Spades}) {
            std::vector<Card> cards{Card(r1, s), Card(r2, s)};
            combos.emplace_back(std::move(cards));
        }
        return combos;
    }
    
    inline std::vector<Hand> getPairCombos(Rank r) {
        std::vector<Hand> combos;
        const Suit suits[] = {Suit::Hearts, Suit::Diamonds, Suit::Clubs, Suit::Spades};

        for (int i = 0; i < 4; ++i) {
            for (int j = i + 1; j < 4; ++j) {
                std::vector<Card> cards{ Card(r, suits[i]), Card(r, suits[j]) };
                combos.emplace_back(std::move(cards));
            }
        }

        return combos;
    }
    
    inline std::vector<Hand> getOffsuitCombos(Rank r1, Rank r2) {
        std::vector<Hand> combos;
        const Suit suits[] = {Suit::Hearts, Suit::Diamonds, Suit::Clubs, Suit::Spades};
        
        for (Suit s1 : suits) {
            for (Suit s2 : suits) {
                if (s1 != s2) {
                    std::vector<Card> cards{Card(r1, s1), Card(r2, s2)};
                    combos.emplace_back(std::move(cards));
                }
            }
        }
        return combos;
    }
    
    inline std::vector<Hand> getAllNonPairCombos(Rank r1, Rank r2) {
        std::vector<Hand> combos = getSuitedCombos(r1,r2);
        std::vector<Hand> offsuit_combos = getOffsuitCombos(r1,r2);
        combos.insert(combos.end(),
            offsuit_combos.begin(), offsuit_combos.end());
        return combos;
    }
}

std::vector<Hand> getHands(const RangeToken& token) {
    std::vector<Hand> combos;
    switch(token.type) {
        case RangeToken::Type::Suited: combos = detail::getSuitedCombos(token.rank1, token.rank2); break;
        case RangeToken::Type::Offsuit: combos = detail::getOffsuitCombos(token.rank1, token.rank2); break;
        case RangeToken::Type::Pair: combos = detail::getPairCombos(token.rank1); break;
        case RangeToken::Type::Normal: combos = detail::getAllNonPairCombos(token.rank1, token.rank2); break;
    }

    return combos;
}

struct RangeNotation {
    std::vector<RangeToken> tokens;
};

namespace literals {
    constexpr RangeToken operator""_r(const char* str, std::size_t size) {
        if (size < 2 || size > 3)
            throw "Invalid range token length";
        
        Rank r1 = detail::char_to_rank(str[0]);
        Rank r2 = detail::char_to_rank(str[1]);

        RangeToken::Type type;
        if (size == 2) {
            type = (r1 == r2) ? RangeToken::Type::Pair : RangeToken::Type::Normal;
        } else {
            if (str[2] == 's')
                type = RangeToken::Type::Suited;
            else if (str[2] == 'o')
                type = RangeToken::Type::Offsuit;
            else
                throw "Invalid suited/offsuit specifier";
        }

        return RangeToken{.rank1 = r1, .rank2 = r2, .type = type};
    }
}

}

#endif