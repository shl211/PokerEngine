#ifndef POKER_ENGINE_UTILS_MATH_HPP
#define POKER_ENGINE_UTILS_MATH_HPP

#include <vector>
#include <iterator>
#include <numeric>
#include <utility>
#include <concepts>

namespace PokerEngine::Utils {

template<std::input_iterator Iter>
auto generate_combinations(Iter begin, Iter end, int k)
    -> std::vector<std::vector<typename std::iterator_traits<Iter>::value_type>>
{
    using T = typename std::iterator_traits<Iter>::value_type;
    std::vector<T> elements(begin, end);
    int n = elements.size();
    if (k < 0 || k > n) return {};

    std::vector<int> indices(k);
    std::iota(indices.begin(), indices.end(), 0);

    std::vector<std::vector<T>> results;
    while(true) {
        std::vector<T> combo;
        combo.reserve(k);
        for (int idx : indices) {
            combo.push_back(elements[idx]);
        }
        results.push_back(std::move(combo));

        // Generate next combination
        int i = k - 1;
        while (i >= 0 && indices[i] == i + n - k) --i;
        if (i < 0) break;
        ++indices[i];
        for (int j = i + 1; j < k; ++j) {
            indices[j] = indices[j - 1] + 1;
        }
    }

    return results;
} 


}

#endif