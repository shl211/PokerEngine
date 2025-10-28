#ifndef POKER_ENGINE_CORE_STACK_HPP
#define POKER_ENGINE_CORE_STACK_HPP

#include <stdexcept>

namespace PokerEngine::Core {

class Stack {

public:
    explicit Stack(int initial_amount);

    void addChips(int amount);
    /**
     * @brief Remove chips, returning amount removed. If amount > value, then all in is returned. 
     * @param amount Amount to remove.
     * @return The actual amount removed. May be smaller than requested amount if player forced to go all in. 
     */
    int removeChips(int amount);
    [[nodiscard]] bool canAfford(int amount) const noexcept { return chips_ >= amount; }
    [[nodiscard]] bool empty() const noexcept { return chips_ == 0; }

    [[nodiscard]] int chips() const { return chips_; }

private:
    int chips_;

};

Stack::Stack(int initial_amount) 
    : chips_(initial_amount) 
{
    if (initial_amount < 0) {
        throw std::invalid_argument("Initial stack cannot be negative");
    }
}

void Stack::addChips(int amount) {
    if (amount < 0)
        throw std::invalid_argument("Cannot add a negative amount");
    chips_ += amount;
}

int Stack::removeChips(int amount) {
    if (amount < 0)
        throw std::invalid_argument("Cannot remove a negative amount");

    if (chips_ == 0) return 0;

    if (amount >= chips_) {
        int allInAmount = chips_;
        chips_ = 0;
        return allInAmount;
    }

    chips_ -= amount;
    return amount;
}
}

#endif