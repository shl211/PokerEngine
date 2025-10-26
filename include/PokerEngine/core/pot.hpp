#ifndef POKER_ENGINE_CORE_POT_HPP
#define POKER_ENGINE_CORE_POT_HPP

#include <unordered_map>
#include <numeric>

namespace PokerEngine::Core {

class Pot {
public:
    using PlayerId = int;

    Pot() = default;

    void addContribution(PlayerId id, int chips);
    int getContribution(PlayerId id);
    int getTotal() const;


    void clear() { contributions_.clear(); }

private:
    std::unordered_map<int, int> contributions_;


};

void Pot::addContribution(PlayerId id, int chips) {
    contributions_[id] += chips;
}

int Pot::getContribution(PlayerId id) {
    int contribution = 0;
    if (auto it = contributions_.find(id); it != contributions_.end()) {
        contribution = it->second;
    }
    return contribution;
}

int Pot::getTotal() const {
    return std::accumulate(contributions_.begin(),contributions_.end(),0,
        [](int sum, const auto& it) { return sum += it.second; }
    );
}

}


#endif