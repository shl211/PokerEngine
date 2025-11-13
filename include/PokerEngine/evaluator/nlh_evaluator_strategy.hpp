#ifndef POKER_ENGINE_EVALUATOR_NLH_EVALUATOR_STRATEGY_HPP
#define POKER_ENGINE_EVALUATOR_NLH_EVALUATOR_STRATEGY_HPP

#include <optional>
#include "PokerEngine/evaluator/detail/bit_mask.hpp"
#include "PokerEngine/evaluator/hand_evaluator_strategy.hpp"

namespace PokerEngine::Evaluator {

inline std::optional<HandRank> check_straight_flush(detail::HandMask mask); 
inline std::optional<HandRank> check_full_house(detail::HandMask mask);
inline std::optional<HandRank> check_quads(detail::HandMask mask);
inline std::optional<HandRank> check_flush(detail::HandMask mask);
inline std::optional<HandRank> check_straight(detail::HandMask mask);
inline std::optional<HandRank> check_three_of_kind(detail::HandMask mask);
inline std::optional<HandRank> check_two_pair(detail::HandMask mask);
inline std::optional<HandRank> check_pair(detail::HandMask mask);
inline std::optional<HandRank> check_high_card(detail::HandMask mask);

HandEvaluationStrategy standardEvaluationStrategy{
    {
    check_straight_flush,
    check_quads,
    check_full_house,
    check_flush,
    check_straight,
    check_three_of_kind,
    check_two_pair,
    check_pair,
    check_high_card
    }
};

HandEvaluationStrategy shortDeckEvaluationStrategy{
    {
    check_straight_flush,
    check_quads,
    check_flush,
    check_full_house,
    check_straight,
    check_three_of_kind,
    check_two_pair,
    check_pair,
    check_high_card
    }
};

HandEvaluationStrategy getStandardEvaluationStrategy() {
    return standardEvaluationStrategy;
}

HandEvaluationStrategy getShortDeckEvaluationStrategy() {
    return shortDeckEvaluationStrategy;
}

namespace {
    uint64_t compute_score(HandType handtype, const std::vector<Core::Card>& besthand) {
        uint64_t s = static_cast<uint64_t>(handtype) << 20; // top bits = hand type

        for (int i = 0; i < besthand.size() && i < 5; ++i) {
            int r = static_cast<int>(besthand[i].rank());
            s |= static_cast<uint64_t>(r) << (16 - i * 4);  // 4 bits per rank
        }

        return s;
    }

    // Returns vector of {rank_index, count}, sorted by count descending, then rank descending
    inline std::vector<std::pair<int,int>> sorted_rank_counts(detail::HandMask mask) {
        auto counts = detail::count_ranks(mask);
        std::vector<std::pair<int,int>> sorted;
        for (int r = 12; r >= 0; --r) {
            if (counts[r] > 0) sorted.emplace_back(r, counts[r]);
        }
        std::sort(sorted.begin(), sorted.end(), [](auto a, auto b){
            return a.second != b.second ? a.second > b.second : a.first > b.first;
        });
        return sorted;
    }

    //Pick N cards of a specific rank (any suit)
    auto pick_rank(detail::HandMask mask, int rank, int n) {
        std::vector<Core::Card> result;
        for (auto& c : detail::mask_cards(mask)) {
            if (static_cast<int>(c.rank()) == rank && result.size() < size_t(n)) {
                result.push_back(c);
            }
        }
        return result;
    }

    //Pick highest remaining cards excluding already chosen
    auto pick_highest(std::vector<Core::Card>& exclude, detail::HandMask mask, int n) {
        std::vector<Core::Card> result;
        auto all = detail::mask_cards(mask);
        for (auto& c : all) {
            if (std::find(exclude.begin(), exclude.end(), c) == exclude.end()) {
                result.push_back(c);
            }
        }
        std::sort(result.begin(), result.end(), [](Core::Card a, Core::Card b){
            return static_cast<int>(a.rank()) > static_cast<int>(b.rank());
        });
        if (result.size() > size_t(n)) result.resize(n);
        return result;
    };
}

std::optional<HandRank> check_straight_flush(detail::HandMask mask) {
    auto [flush_suit, flush_mask] = detail::find_flush(mask);
    if (flush_suit < 0) return std::nullopt;

    auto sf_res = detail::is_straight(static_cast<detail::RankMask>(flush_mask));
    if (!sf_res.is_straight) return std::nullopt;

    HandType hand_type = (sf_res.high_card == Core::Rank::Ace) ? HandType::RoyalFlush : HandType::StraightFlush;

    std::vector<Core::Card> best_hand;
    for (int i = 0; i < 5; ++i) {
        int rank = static_cast<int>(sf_res.high_card) - i;
        best_hand.push_back(Core::Card{detail::rank_from_index(rank - 2), static_cast<Core::Suit>(flush_suit)});
    }

    return HandRank{hand_type, best_hand, compute_score(hand_type, best_hand)};
}

std::optional<HandRank> check_quads(detail::HandMask mask) {
    auto sorted_counts = sorted_rank_counts(mask);
    auto quad = sorted_counts[0];
    if (quad.second != 4) return std::nullopt;

    HandType hand_type = HandType::FourOfAKind;
    std::vector<Core::Card> best_hand = pick_rank(mask, quad.first + 2, 4);
    auto kicker = pick_highest(best_hand, mask, 1);
    best_hand.insert(best_hand.end(), kicker.begin(), kicker.end());
    return HandRank{hand_type, best_hand, compute_score(hand_type, best_hand)};
}


std::optional<HandRank> check_full_house(detail::HandMask mask) {
    auto sorted_counts = sorted_rank_counts(mask);
    if (sorted_counts.size() < 2) return std::nullopt;
    const auto& three = sorted_counts[0];
    const auto& pair = sorted_counts[1];

    if (three.second != 3 || pair.second < 2) return std::nullopt;

    HandType hand_type = HandType::FullHouse;
    std::vector<Core::Card> best_hand = pick_rank(mask, three.first + 2, 3);
    auto pair_cards = pick_rank(mask, pair.first + 2, 2);
    best_hand.insert(best_hand.end(), pair_cards.begin(), pair_cards.end());

    return HandRank{hand_type, best_hand, compute_score(hand_type, best_hand)};
}


std::optional<HandRank> check_flush(detail::HandMask mask) {
    auto [flush_suit, flush_mask] = detail::find_flush(mask);

    if (flush_suit < 0) return std::nullopt;

    HandType hand_type = HandType::Flush;
    std::vector<Core::Card> best_hand;

    for (int r = 12; r >= 0 && best_hand.size() < 5; --r) {
        if (flush_mask & (1 << r)) {
            best_hand.push_back(Core::Card{detail::rank_from_index(r), static_cast<Core::Suit>(flush_suit)});
        }
    }

    if (best_hand.empty()) return std::nullopt;
    return HandRank{hand_type, best_hand, compute_score(hand_type, best_hand)};
}

std::optional<HandRank> check_straight(detail::HandMask mask) {
    detail::RankMask r_mask = detail::rank_mask(mask);
    auto straight_res = detail::is_straight(r_mask);
    if (!straight_res.is_straight) return std::nullopt;

    HandType hand_type = HandType::Straight;
    std::vector<Core::Card> best_hand;

    if (straight_res.high_card == Core::Rank::Five) { // Ace-low straight
        for (int rank : {5,4,3,2,14}) {
            auto cands = pick_rank(mask, rank, 1);
            if (!cands.empty()) best_hand.push_back(cands[0]);
        }
    } else {
        for (int i = 0; i < 5; ++i) {
            int rank = static_cast<int>(straight_res.high_card) - i;
            auto cands = pick_rank(mask, rank, 1);
            if (!cands.empty()) best_hand.push_back(cands[0]);
        }
    }

    if (best_hand.empty()) return std::nullopt;
    return HandRank{hand_type, best_hand, compute_score(hand_type, best_hand)};
}

std::optional<HandRank> check_three_of_kind(detail::HandMask mask) {
    auto sorted_counts = sorted_rank_counts(mask);
    if (sorted_counts.empty() || sorted_counts[0].second != 3) return std::nullopt;

    HandType hand_type = HandType::ThreeOfAKind;
    std::vector<Core::Card> best_hand = pick_rank(mask, sorted_counts[0].first + 2, 3);

    auto kickers = pick_highest(best_hand, mask, 2);
    best_hand.insert(best_hand.end(), kickers.begin(), kickers.end());

    return HandRank{hand_type, best_hand, compute_score(hand_type, best_hand)};
}

std::optional<HandRank> check_two_pair(detail::HandMask mask) {
    auto sorted_counts = sorted_rank_counts(mask);
    if (sorted_counts.size() < 2) return std::nullopt;
    const auto& first_pair = sorted_counts[0];
    const auto& second_pair = sorted_counts[1];

    if (first_pair.second != 2 || second_pair.second != 2) return std::nullopt;

    HandType hand_type = HandType::TwoPair;
    std::vector<Core::Card> best_hand = pick_rank(mask, first_pair.first + 2, 2);
    auto pair2 = pick_rank(mask, second_pair.first + 2, 2);
    best_hand.insert(best_hand.end(), pair2.begin(), pair2.end());

    auto kicker = pick_highest(best_hand, mask, 1);
    best_hand.insert(best_hand.end(), kicker.begin(), kicker.end());

    return HandRank{hand_type, best_hand, compute_score(hand_type, best_hand)};
}

std::optional<HandRank> check_pair(detail::HandMask mask) {
    auto sorted_counts = sorted_rank_counts(mask);
    if (sorted_counts.empty() || sorted_counts[0].second != 2) return std::nullopt;

    HandType hand_type = HandType::OnePair;
    std::vector<Core::Card> best_hand = pick_rank(mask, sorted_counts[0].first + 2, 2);

    auto kickers = pick_highest(best_hand, mask, 3);
    best_hand.insert(best_hand.end(), kickers.begin(), kickers.end());

    return HandRank{hand_type, best_hand, compute_score(hand_type, best_hand)};
}

std::optional<HandRank> check_high_card(detail::HandMask mask) {
    std::vector<Core::Card> best_hand;
    auto hand_type = HandType::HighCard;
    best_hand = pick_highest(best_hand, mask, 5);
    return HandRank{hand_type, best_hand, compute_score(hand_type, best_hand)};
}
}

#endif