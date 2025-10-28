#include <gtest/gtest.h>

#include "PokerEngine/core/pot.hpp"

using namespace PokerEngine::Core;

TEST(Pot, GettingWinningsFromPotAllEqual) {
    
    Pot pot;
    pot.addContribution(Pot::PlayerId{1}, 100);
    pot.addContribution(Pot::PlayerId{2}, 100);
    pot.addContribution(Pot::PlayerId{3}, 100);

    int p1_winnings = pot.getWinningsForPlayer(Pot::PlayerId{1});
    ASSERT_EQ(p1_winnings, 300);
    ASSERT_TRUE(pot.empty());
}

TEST(Pot, GettingWinningsFromPotSidePotInvolved) {
    
    Pot pot;
    pot.addContribution(Pot::PlayerId{1}, 100);
    pot.addContribution(Pot::PlayerId{2}, 200);
    pot.addContribution(Pot::PlayerId{3}, 200);

    int p1_winnings = pot.getWinningsForPlayer(Pot::PlayerId{1});
    ASSERT_EQ(p1_winnings, 300);
    ASSERT_FALSE(pot.empty());

    int p2_winnings = pot.getWinningsForPlayer(Pot::PlayerId{2});
    ASSERT_EQ(p2_winnings, 200);
    ASSERT_TRUE(pot.empty());
}