#include <gtest/gtest.h>

#include <vector>
#include <string>
#include <list>
#include "PokerEngine/utils/math.hpp"
#include "PokerEngine/core/card.hpp"

using namespace PokerEngine::Utils;

TEST(Combinations, IntegersNormal) {
    std::vector<int> v = {1, 2, 3, 4};
    auto combos = generate_combinations(v.begin(), v.end(), 2);

    std::vector<std::vector<int>> expected{ {1,2}, {1,3}, {1,4}, {2,3}, {2,4}, {3,4}};
    ASSERT_EQ(combos.size(), expected.size());
    ASSERT_EQ(combos, expected);
}

TEST(Combinations, IntegersEmpty) {
    std::vector<int> v = {1, 2, 3, 4};
    auto combos = generate_combinations(v.begin(), v.end(), 0);

    std::vector<std::vector<int>> expected{{}};
    ASSERT_EQ(combos.size(), expected.size());
    ASSERT_EQ(combos, expected);
}

TEST(Combinations, KGreaterThanN) {
    std::vector<int> v = {1, 2, 3, 4};
    auto combos = generate_combinations(v.begin(), v.end(), 5);

    ASSERT_TRUE(combos.empty());
}

TEST(Combinations, KEqualsN) {
    std::vector<int> v = {1, 2, 3, 4};
    auto combos = generate_combinations(v.begin(), v.end(), 4);

    std::vector<std::vector<int>> expected{{1, 2, 3, 4}};
    ASSERT_EQ(combos, expected);
}

TEST(Combinations, KNegative) {
    std::vector<int> v = {1, 2, 3, 4};
    auto combos = generate_combinations(v.begin(), v.end(), -1);

    ASSERT_TRUE(combos.empty());
}

TEST(Combinations, EmptyInput) {
    std::vector<int> v;
    auto combos = generate_combinations(v.begin(), v.end(), 2);
    ASSERT_TRUE(combos.empty());
}

TEST(Combinations, EmptyInputKZero) {
    std::vector<int> v;
    auto combos = generate_combinations(v.begin(), v.end(), 0);

    std::vector<std::vector<int>> expected{{}};
    ASSERT_EQ(combos, expected);
}

TEST(Combinations, RawPointerIterator) {
    int arr[] = {10, 20, 30};
    auto combos = generate_combinations(std::begin(arr), std::end(arr), 2);

    std::vector<std::vector<int>> expected{{10, 20}, {10, 30}, {20, 30}};
    ASSERT_EQ(combos, expected);
}

TEST(Combinations, ConstIterator) {
    const std::vector<int> v = {1, 2, 3};
    auto combos = generate_combinations(v.cbegin(), v.cend(), 2);

    std::vector<std::vector<int>> expected{{1, 2}, {1, 3}, {2, 3}};
    ASSERT_EQ(combos, expected);
}

TEST(Combinations, ListIterator) {
    std::list<int> lst = {1, 2, 3, 4};
    auto combos = generate_combinations(lst.begin(), lst.end(), 2);

    std::vector<std::vector<int>> expected{{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}};
    ASSERT_EQ(combos, expected);
}

TEST(Combinations, StringElements) {
    std::vector<std::string> v = {"A", "B", "C"};
    auto combos = generate_combinations(v.begin(), v.end(), 2);

    std::vector<std::vector<std::string>> expected{{"A", "B"}, {"A", "C"}, {"B", "C"}};
    ASSERT_EQ(combos, expected);
}

TEST(Combinations, CardElements) {
    using namespace PokerEngine::Core::literals;
    std::vector<PokerEngine::Core::Card> v = {"Ac"_c, "Th"_c, "Js"_c};
    auto combos = generate_combinations(v.begin(), v.end(), 2);

    std::vector<std::vector<PokerEngine::Core::Card>> expected{
        {"Ac"_c, "Th"_c},
        {"Ac"_c, "Js"_c},
        {"Th"_c, "Js"_c}
    };
    ASSERT_EQ(combos, expected);
}