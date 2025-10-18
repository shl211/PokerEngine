#ifndef POKER_ENGINE_EVALUATOR_DETAIL_BIT_MASK_HPP
#define POKER_ENGINE_EVALUATOR_DETAIL_BIT_MASK_HPP

#include <cstdint>
#include <vector>
#include <array>
#include "PokerEngine/core/card.hpp"

namespace PokerEngine::Evaluator::detail {

using Core::Card;
using HandMask = std::uint64_t;
using RankMask = std::uint16_t;

namespace {
    constexpr void assert_rank_enum_values_at_compile_time() {
        //this checks main assumption that Two = 2, Three = 3 ... Ace = 14
        static_assert(static_cast<int>(Core::Rank::Two)   == 2,  "Rank::Two must be 2");
        static_assert(static_cast<int>(Core::Rank::Three) == 3,  "Rank::Three must be 3");
        static_assert(static_cast<int>(Core::Rank::Four)  == 4,  "Rank::Four must be 4");
        static_assert(static_cast<int>(Core::Rank::Five)  == 5,  "Rank::Five must be 5");
        static_assert(static_cast<int>(Core::Rank::Six)   == 6,  "Rank::Six must be 6");
        static_assert(static_cast<int>(Core::Rank::Seven) == 7,  "Rank::Seven must be 7");
        static_assert(static_cast<int>(Core::Rank::Eight) == 8,  "Rank::Eight must be 8");
        static_assert(static_cast<int>(Core::Rank::Nine)  == 9,  "Rank::Nine must be 9");
        static_assert(static_cast<int>(Core::Rank::Ten)   == 10, "Rank::Ten must be 10");
        static_assert(static_cast<int>(Core::Rank::Jack)  == 11, "Rank::Jack must be 11");
        static_assert(static_cast<int>(Core::Rank::Queen) == 12, "Rank::Queen must be 12");
        static_assert(static_cast<int>(Core::Rank::King)  == 13, "Rank::King must be 13");
        static_assert(static_cast<int>(Core::Rank::Ace)   == 14, "Rank::Ace must be 14");    
    }

    constexpr inline Core::Rank rank_from_index(int index) noexcept {
        return static_cast<Core::Rank>(index + 2);
    }
}

constexpr HandMask SUIT_MASK = 0x1FFF;
constexpr HandMask STRAIGHT_MASK = 0b11111U;
constexpr uint16_t ACE_LOW_STRAIGHT_MASK = 0b1000000001111U;

struct StraightResult {
    bool is_straight;
    Core::Rank high_card;
};

constexpr inline HandMask card_bitmask(Card card) noexcept {
    assert_rank_enum_values_at_compile_time();
    HandMask suit_offset = static_cast<HandMask>(card.suit()) * 13;
    HandMask rank_index = static_cast<HandMask>(card.rank()) - 2;
    return HandMask{1} << (suit_offset + rank_index);
}

constexpr inline HandMask cards_bitmask(const std::vector<Card>& cards) noexcept {
    HandMask mask = 0;
    for(auto& c : cards) {
        mask |= card_bitmask(c);
    }

    return mask;
}

constexpr inline HandMask suit_mask(HandMask hand_mask, Core::Suit suit) noexcept {
    return (hand_mask >> (static_cast<HandMask>(suit) * 13)) & SUIT_MASK;
}

constexpr inline RankMask rank_mask(HandMask hand_mask) noexcept {
    return (hand_mask & 0x1FFF)          |  // suit 0
           ((hand_mask >> 13) & 0x1FFF)  |  // suit 1
           ((hand_mask >> 26) & 0x1FFF)  |  // suit 2
           ((hand_mask >> 39) & 0x1FFF);    // suit 3
}

constexpr inline auto count_ranks(HandMask hand_mask) noexcept {
    std::array<int, 13> counts{};
    for(int rank = 0; rank < 13; ++rank) {
        for(int suit = 0; suit < 4; ++suit) {
            if(hand_mask & (HandMask{1} << (suit * 13 + rank))) {
                counts[rank]++;
            }
        }
    }

    return counts;
}

constexpr inline StraightResult is_straight(HandMask) = delete; //avoid accidentally using HandMask here
constexpr inline StraightResult is_straight(RankMask rank_mask) noexcept {
    // Normal straight
    for (int start = 8; start >= 0; --start) {
        if (((rank_mask >> start) & STRAIGHT_MASK) == STRAIGHT_MASK) {
            auto straight_high_card = start + 4;
            return StraightResult{true, rank_from_index(straight_high_card)};
        }
    }
    // Ace-low straight
    if ((rank_mask & ACE_LOW_STRAIGHT_MASK) == ACE_LOW_STRAIGHT_MASK) {
        return StraightResult{true, Core::Rank::Five};
    }

    constexpr auto default_invalid_rank = Core::Rank::Two;
    return {false, default_invalid_rank};
}

inline std::vector<Card> mask_cards(HandMask mask) {
    std::vector<Card> cards;
    for (int suit = 0; suit < 4; ++suit) {
        for (int rank = 0; rank < 13; ++rank) {
            if (mask & (HandMask{1} << (suit * 13 + rank))) {
                cards.push_back(Card{static_cast<Core::Rank>(rank + 2),
                                    static_cast<Core::Suit>(suit)});
            }
        }
    }
    return cards;
}

}


#endif