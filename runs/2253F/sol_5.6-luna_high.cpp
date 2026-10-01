#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

static constexpr int64 INF = (1LL << 62);

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    vector<int64> cost(n + 1);
    for (int i = 1; i <= n; ++i) {
        cin >> cost[i];
    }

    int64 answer = 0;

    // Every number has a unique representation b * 2^i * 3^j,
    // where gcd(b, 6) = 1.  A constraint {x, 2x, 3x, 4x}
    // never connects different values of b.
    for (int base = 1; 4LL * base <= n; ++base) {
        if (base % 2 == 0 || base % 3 == 0) {
            continue;
        }

        vector<int64> powers2{1}, powers3{1};
        while (1LL * base * powers2.back() * 2 <= n) {
            powers2.push_back(powers2.back() * 2);
        }
        while (1LL * base * powers3.back() * 3 <= n) {
            powers3.push_back(powers3.back() * 3);
        }

        const int width = static_cast<int>(powers2.size());
        const int height = static_cast<int>(powers3.size());
        const int full = (1 << width) - 1;
        const int states = 1 << width;

        vector<int> shifted(states);
        for (int mask = 0; mask < states; ++mask) {
            shifted[mask] = (mask << 1) & full;
        }

        vector<int64> dp(states, INF), next(states, INF);
        dp[0] = 0;

        // Scan the exponent grid row by row.  The mask stores the last
        // width keep/delete decisions.  At column col of a non-first row,
        // its three highest bits are the cells directly above it and the
        // next two cells to the right.
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                fill(next.begin(), next.end(), INF);

                const int64 value = base * powers3[row] * powers2[col];
                const bool exists = value <= n;
                const int64 remove_cost = exists ? cost[static_cast<int>(value)] : 0;
                const bool can_complete_edge = row > 0 && col + 2 < width;
                const int top_three = can_complete_edge ? (7 << (width - 3)) : 0;

                for (int mask = 0; mask < states; ++mask) {
                    if (dp[mask] == INF) {
                        continue;
                    }

                    const int out = shifted[mask];

                    // Delete the current number.  Missing grid cells are
                    // treated as already deleted and have zero cost.
                    next[out] = min(next[out], dp[mask] + remove_cost);

                    // Keep the current number, unless this would complete
                    // an unbroken {x, 2x, 3x, 4x}.
                    if (exists && (!can_complete_edge || (mask & top_three) != top_three)) {
                        next[out | 1] = min(next[out | 1], dp[mask]);
                    }
                }

                dp.swap(next);
            }
        }

        answer += *min_element(dp.begin(), dp.end());
    }

    cout << answer << '\n';
    return 0;
}
