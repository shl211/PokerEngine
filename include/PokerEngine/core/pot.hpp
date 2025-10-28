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
    int getWinningsForPlayer(PlayerId id);

    bool empty() { return contributions_.empty() || getTotal() == 0; }
    void clear() { contributions_.clear(); }

private:
    std::unordered_map<PlayerId, int> contributions_;
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

/**
 * @brief Given player id, their winnings will be returned. They can only win as much money from other players as they have put into the pot.
 * 
 */
int Pot::getWinningsForPlayer(PlayerId id) {
    int player_max_winning_per_player = contributions_[id];
    contributions_[id] = 0;
    int total = player_max_winning_per_player;
    for(auto [pid, chips] : contributions_) {
        if(pid != id) {
            int winnings_from_pid = chips < player_max_winning_per_player ? chips : player_max_winning_per_player;
            contributions_[pid] -= winnings_from_pid;
            total += winnings_from_pid;
        }
    }

    return total;
}

}


#endif