#include <gtest/gtest.h>

#include "PokerEngine/core/stack.hpp"

TEST(StackTest, ConstructsWithPositiveAmount) {
    PokerEngine::Core::Stack s(1000);
    EXPECT_EQ(s.chips(), 1000);
}

TEST(StackTest, ThrowsOnNegativeInitialAmount) {
    EXPECT_THROW(PokerEngine::Core::Stack(-50), std::invalid_argument);
}

TEST(StackTest, AddChipsIncreasesBalance) {
    PokerEngine::Core::Stack s(100);
    s.addChips(50);
    EXPECT_EQ(s.chips(), 150);
}

TEST(StackTest, AddChipsThrowsOnNegative) {
    PokerEngine::Core::Stack s(100);
    EXPECT_THROW(s.addChips(-10), std::invalid_argument);
}

TEST(StackTest, RemoveChipsReducesBalance) {
    PokerEngine::Core::Stack s(200);
    int removed = s.removeChips(50);
    EXPECT_EQ(removed, 50);
    EXPECT_EQ(s.chips(), 150);
}

TEST(StackTest, RemoveChipsAllInWhenAmountExceedsStack) {
    PokerEngine::Core::Stack s(100);
    int removed = s.removeChips(200);
    EXPECT_EQ(removed, 100);     // all-in
    EXPECT_EQ(s.chips(), 0);
}

TEST(StackTest, RemoveChipsFromEmptyStackReturnsZero) {
    PokerEngine::Core::Stack s(0);
    int removed = s.removeChips(50);
    EXPECT_EQ(removed, 0);
    EXPECT_EQ(s.chips(), 0);
}

TEST(StackTest, RemoveChipsThrowsOnNegativeAmount) {
    PokerEngine::Core::Stack s(100);
    EXPECT_THROW(s.removeChips(-1), std::invalid_argument);
}

TEST(StackTest, CanAffordReturnsTrueIfEnoughChips) {
    PokerEngine::Core::Stack s(100);
    EXPECT_TRUE(s.canAfford(50));
    EXPECT_FALSE(s.canAfford(200));
}

TEST(StackTest, EmptyReturnsTrueOnlyIfZero) {
    PokerEngine::Core::Stack s(10);
    EXPECT_FALSE(s.empty());
    s.removeChips(10);
    EXPECT_TRUE(s.empty());
}