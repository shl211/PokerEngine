#ifndef POKER_ENGINE_CORE_RANGE_HPP
#define POKER_ENGINE_CORE_RANGE_HPP

#include <vector>
#include <algorithm>
#include <ranges>
#include <numeric>
#include <optional>
#include <random>

#include "PokerEngine/core/card.hpp"

namespace PokerEngine::Core {

struct Combo {
    Combo(Card c1, Card c2, double weight)
        : weight(weight) 
    {
        if (c2 < c1) std::swap(c1, c2);
        this->c1 = c1;
        this->c2 = c2;
    }

    Card c1, c2;
    double weight;
};

constexpr inline bool operator==(const Combo& lhs, const Combo& rhs) noexcept {
    return lhs.c1 == rhs.c1 && lhs.c2 == rhs.c2;
}

class Range {
public:
    Range() = default;

    void addCombo(Card c1, Card c2, double weight = 1.0);
    void removeBlocked(const std::vector<Card>& known);

    std::optional<Combo> sample(std::mt19937& rng) const;

    const std::vector<Combo>& combos() const noexcept {return combos_;}
    size_t size() const noexcept { return combos_.size(); }

private:
    std::vector<Combo> combos_;

};

void Range::addCombo(Card c1, Card c2, double weight) {
    Combo combo{c1,c2,weight};

    if (std::ranges::find(combos_, combo) == combos_.end()) {
        combos_.push_back(combo);
    }
}

//TODO: this needs to be more efficient -> don't actually erase, just mark as erased
//The search is O(N)
void Range::removeBlocked(const std::vector<Card>& known) {
    std::erase_if(combos_, 
        [&](const Combo& c) {
            return std::ranges::find(known, c.c1) != known.end() ||
                std::ranges::find(known, c.c2) != known.end();
        }
    );
}

//TODO: replace with prefix sums and O(logN) sampling
std::optional<Combo> Range::sample(std::mt19937& rng) const {
    if(combos_.empty()) return std::nullopt;

    double total_weight = std::accumulate(
        combos_.begin(), combos_.end(), 0.0,
        [](double sum, const Combo& c) {return sum + c.weight;}
    );

    std::uniform_real_distribution<double> dist(0.0, total_weight);
    double pick = dist(rng);

    for (const auto& c : combos_) {
        pick -= c.weight;
        if(pick <= 0.0) return c; 
    }

    return combos_.back(); // fallback
}


}


#endif