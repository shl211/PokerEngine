#include <gtest/gtest.h>

#include "PokerEngine/core/range.hpp"

using namespace PokerEngine::Core::literals;
using PokerEngine::Core::Card;
using PokerEngine::Core::Range;

TEST(RangeTest, CorrectInitialisation) {

    Range r{};
    Card a{"Ah"_c}, b{"Kd"_c}, c{"Qs"_c};

    r.addCombo(a,b);
    EXPECT_EQ(r.size(), 1) << "Added one combo";

    r.addCombo(a,c);
    EXPECT_EQ(r.size(), 2) << "Added second combo";
}

TEST(RangeTest, BlockerRemoved) {
    Range r{};
    Card a{"Ah"_c}, b{"Kd"_c}, c{"Qs"_c};

    r.addCombo(a,b);
    r.addCombo(a,c);
    r.addCombo(b,c);

    EXPECT_EQ(r.size(), 3) << "Three combos";
    r.removeBlocked({a});
    EXPECT_EQ(r.size(), 1) << "Blocks 2 hands, should only have one left";
}

TEST(RangeTest, Sampling) {
    std::mt19937 rng(42);
    Range r;
    Card a{"Ah"}, b{"Kd"}, c{"Qs"}, d{"Jc"};
    r.addCombo(a,b, 1.0);
    r.addCombo(c,d, 3.0);

    int count_ab = 0;
    int count_cd = 0;
    int iterations = 10000;

    for(int i = 0; i < iterations; ++i){
        auto sampled = r.sample(rng);
        ASSERT_TRUE(sampled.has_value());
        if(sampled == PokerEngine::Core::Combo{a,b,1}) count_ab++;
        if(sampled == PokerEngine::Core::Combo{c,d,3}) count_cd++;
    }

    EXPECT_GT(count_cd, count_ab);

    // Expect roughly 3x more occurrences for 'c,d' than 'a,b', allowing some tolerance
    double ratio = static_cast<double>(count_cd) / static_cast<double>(count_ab);
    EXPECT_NEAR(ratio, 3.0, 0.2); // tolerance of ±0.2
}

TEST(RangeTest, SampleEmpty) {
    Range r;
    std::mt19937 rng(42);
    auto s = r.sample(rng);
    EXPECT_FALSE(s.has_value());
}

TEST(RangeTest, SampleSingleCombo) {
    Range r{};
    std::mt19937 rng(42);
    Card a{"Ah"}, b{"Kd"};
    r.addCombo(a,b, 1.0);
    for(int i=0;i<10;++i){
        auto s = r.sample(rng);
        ASSERT_TRUE(s.has_value());
        EXPECT_EQ(*s, PokerEngine::Core::Combo(a,b,1.0));
    }
}