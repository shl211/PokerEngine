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
    int getContribution(PlayerId id) const;
    int getTotal() const;
    int getWinningsForPlayer(PlayerId id);

    bool empty() { return contributions_.empty() || getTotal() == 0; }
    void clear() { 
        contributions_.clear();
        pot_total_ = 0;
    }

private:
    std::unordered_map<PlayerId, int> contributions_;
    int pot_total_ = 0;

    void removeContribution(PlayerId id, int chips_to_remove);
};

void Pot::addContribution(PlayerId id, int chips) {
    contributions_[id] += chips;
    pot_total_ += chips;
}

int Pot::getContribution(PlayerId id) const {
    int contribution = 0;
    if (auto it = contributions_.find(id); it != contributions_.end()) {
        contribution = it->second;
    }
    return contribution;
}

int Pot::getTotal() const {
    return pot_total_;
}

void Pot::removeContribution(PlayerId id, int chips_to_remove) {
    if(contributions_.contains(id)) {
        int contribution = contributions_[id];
        int to_remove = std::min(chips_to_remove, contribution);
        contributions_[id] -= to_remove;
        pot_total_ -= to_remove;
    }
}

/**
 * @brief Given player id, their winnings will be returned. They can only win as much money from other players as they have put into the pot.
 * 
 */
int Pot::getWinningsForPlayer(PlayerId id) {
    int player_max_winning_per_player = contributions_[id];
    removeContribution(id, player_max_winning_per_player);
    int total = player_max_winning_per_player;
    for(auto [pid, chips] : contributions_) {
        if(pid != id) {
            int winnings_from_pid = std::min(chips, player_max_winning_per_player);
            removeContribution(pid, winnings_from_pid);
            total += winnings_from_pid;
        }
    }

    return total;
}

}


#endif