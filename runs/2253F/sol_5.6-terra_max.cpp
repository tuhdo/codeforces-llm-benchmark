#include <bits/stdc++.h>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    vector<long long> remove_cost(n + 1);
    for (int i = 1; i <= n; ++i) cin >> remove_cost[i];

    int max_width = 1;
    while ((1 << max_width) <= n) ++max_width;
    const int max_masks = 1 << max_width;
    const long long inf = (1LL << 62);

    vector<long long> dp(max_masks), next_dp(max_masks), best(max_masks), subset_cost(max_masks);

    auto row_width = [&](int base) {
        return 32 - __builtin_clz(n / base);
    };

    auto build_subset_costs = [&](int base, int width) {
        long long weight[20];
        int value = base;
        for (int i = 0; i < width; ++i) {
            weight[i] = remove_cost[value];
            value *= 2;
        }

        const int masks = 1 << width;
        subset_cost[0] = 0;
        for (int mask = 1; mask < masks; ++mask) {
            const int bit = __builtin_ctz(mask);
            subset_cost[mask] = subset_cost[mask ^ (1 << bit)] + weight[bit];
        }
    };

    long long answer = 0;

    // Numbers in different components after removing all factors 2 and 3
    // never occur in the same quadruple.
    for (int root = 1; root * 4 <= n; ++root) {
        if (root % 2 == 0 || root % 3 == 0) continue;

        int base = root;
        int width = row_width(base);
        int masks = 1 << width;
        build_subset_costs(base, width);
        for (int mask = 0; mask < masks; ++mask) dp[mask] = subset_cost[mask];

        // A row is base * 2^a.  The next row is 3 * base * 2^a.
        // An edge starting at a requires removing at least one of
        // base*2^a, base*2^(a+1), base*2^(a+2), 3*base*2^a.
        while (base * 4 <= n) {
            const int next_base = base * 3;
            const int next_width = row_width(next_base);
            const int next_masks = 1 << next_width;
            build_subset_costs(next_base, next_width);

            fill(best.begin(), best.begin() + next_masks, inf);
            const int full_mask = masks - 1;

            for (int mask = 0; mask < masks; ++mask) {
                // For every uncovered horizontal triple, the corresponding
                // position in the next row is forced to be removed.
                const int zeroes = (~mask) & full_mask;
                const int forced = zeroes & (zeroes >> 1) & (zeroes >> 2);
                best[forced] = min(best[forced], dp[mask]);
            }

            // best[mask] becomes the least prior cost whose forced set is a
            // subset of mask.
            for (int bit = 0; bit < next_width; ++bit) {
                const int bit_value = 1 << bit;
                for (int mask = 0; mask < next_masks; ++mask) {
                    if (mask & bit_value) {
                        best[mask] = min(best[mask], best[mask ^ bit_value]);
                    }
                }
            }

            for (int mask = 0; mask < next_masks; ++mask) {
                next_dp[mask] = best[mask] + subset_cost[mask];
            }

            dp.swap(next_dp);
            base = next_base;
            width = next_width;
            masks = next_masks;
        }

        long long component_answer = inf;
        for (int mask = 0; mask < masks; ++mask) {
            component_answer = min(component_answer, dp[mask]);
        }
        answer += component_answer;
    }

    cout << answer << '\n';
    return 0;
}
