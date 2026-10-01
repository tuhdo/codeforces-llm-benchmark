#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

using i64 = long long;

static i64 solve_one_dimensional(const std::vector<i64>& a, int k) {
    const int len = static_cast<int>(a.size());
    const i64 sum = [&] {
        i64 result = 0;
        for (i64 value : a) result += value;
        return result;
    }();

    if (len == 1) return a[0] >= 0 ? 0 : -1;

    // b[i] >= 0 is exactly the peak condition in a one-dimensional matrix.
    std::vector<i64> b(len);
    for (int i = 0; i < len; ++i) b[i] = 2 * a[i] - sum;

    const i64 q = len - 2;
    std::vector<i64> inner(b.begin() + 1, b.end() - 1);
    std::sort(inner.begin(), inner.end());

    auto peak_count = [&](i64 operations, i64 endpoint_bias) {
        const i64 base = operations * q;
        const i64 inner_threshold = std::llabs(endpoint_bias) - base;
        const auto first = std::lower_bound(inner.begin(), inner.end(), inner_threshold);
        i64 count = static_cast<i64>(inner.end() - first);
        if (b.front() + base + endpoint_bias >= 0) ++count;
        if (b.back() + base - endpoint_bias >= 0) ++count;
        return count;
    };

    auto feasible = [&](i64 operations) {
        const i64 base = operations * q;
        // The left endpoint is a peak iff e >= left_limit, and the right
        // endpoint is a peak iff e <= right_limit.
        const i64 left_limit = -b.front() - base;
        const i64 right_limit = b.back() + base;

        std::vector<i64> candidates = {0, -operations, operations};
        for (i64 value : {left_limit - 1, left_limit, left_limit + 1,
                          right_limit - 1, right_limit, right_limit + 1}) {
            candidates.push_back(std::clamp(value, -operations, operations));
        }

        i64 best = 0;
        for (i64 endpoint_bias : candidates)
            best = std::max(best, peak_count(operations, endpoint_bias));
        return best >= k;
    };

    if (feasible(0)) return 0;

    i64 low = 0;
    i64 high = 1;
    while (!feasible(high)) high *= 2;
    while (high - low > 1) {
        const i64 middle = low + (high - low) / 2;
        if (feasible(middle)) high = middle;
        else low = middle;
    }
    return high;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int tests;
    std::cin >> tests;
    while (tests--) {
        int n, m, k;
        std::cin >> n >> m >> k;

        std::vector<i64> a(static_cast<size_t>(n) * m);
        for (i64& value : a) std::cin >> value;

        if (n == 1 || m == 1) {
            std::cout << solve_one_dimensional(a, k) << '\n';
            continue;
        }

        std::vector<i64> row_sum(n, 0), column_sum(m, 0);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                const i64 value = a[static_cast<size_t>(i) * m + j];
                row_sum[i] += value;
                column_sum[j] += value;
            }
        }

        const i64 gain_per_operation = n + m - 3;
        std::vector<i64> required;
        required.reserve(a.size());
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                const i64 value = a[static_cast<size_t>(i) * m + j];
                const i64 score = 3 * value - row_sum[i] - column_sum[j];
                if (score >= 0) required.push_back(0);
                else required.push_back((-score + gain_per_operation - 1) /
                                        gain_per_operation);
            }
        }

        std::nth_element(required.begin(), required.begin() + (k - 1), required.end());
        std::cout << required[k - 1] << '\n';
    }
    return 0;
}
