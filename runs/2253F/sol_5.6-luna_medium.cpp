#include <bits/stdc++.h>

using namespace std;

using int64 = long long;

static constexpr int64 INF = (1LL << 62);

int n;
vector<int64> cost;

// Solve one component consisting of numbers base * 2^x * 3^y.
// A forbidden quadruple is the four cells
// (x, y), (x + 1, y), (x + 2, y), (x, y + 1).
int64 solve_component(int base) {
    vector<int64> powers2{1}, powers3{1};

    while (1LL * base * powers2.back() * 2 <= n)
        powers2.push_back(powers2.back() * 2);
    while (1LL * base * powers3.back() * 3 <= n)
        powers3.push_back(powers3.back() * 3);

    const int width = static_cast<int>(powers2.size());
    const int full_mask = (1 << width) - 1;

    // A 1 bit means that the corresponding processed number is kept.
    // The most significant bit is the oldest cell in the profile.
    vector<int64> dp(1 << width, INF), next(1 << width, INF);
    dp[0] = 0;

    for (int row = 0; row < static_cast<int>(powers3.size()); ++row) {
        for (int column = 0; column < width; ++column) {
            fill(next.begin(), next.end(), INF);

            const int64 value = 1LL * base * powers3[row] * powers2[column];
            const bool exists = value <= n;

            for (int mask = 0; mask <= full_mask; ++mask) {
                if (dp[mask] == INF)
                    continue;

                const int shifted = (mask << 1) & full_mask;

                // Delete this number.
                next[shifted] = min(next[shifted],
                                    dp[mask] + (exists ? cost[value] : 0));

                if (!exists)
                    continue;

                // Keeping this cell is forbidden if the three cells directly
                // above it are kept: they form the other part of a bad set.
                const bool completes_bad_set =
                    row > 0 && column + 2 < width &&
                    ((mask >> (width - 1)) & 1) &&
                    ((mask >> (width - 2)) & 1) &&
                    ((mask >> (width - 3)) & 1);

                if (!completes_bad_set)
                    next[shifted | 1] = min(next[shifted | 1], dp[mask]);
            }

            dp.swap(next);
        }
    }

    return *min_element(dp.begin(), dp.end());
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> n;
    cost.assign(n + 1, 0);
    for (int i = 1; i <= n; ++i)
        cin >> cost[i];

    int64 answer = 0;
    for (int base = 1; 4LL * base <= n; ++base) {
        if (base % 2 != 0 && base % 3 != 0)
            answer += solve_component(base);
    }

    cout << answer << '\n';
    return 0;
}
