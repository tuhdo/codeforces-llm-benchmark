#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

// Every number in a component is root * 3^row * 2^column.
static int64 solve_component(int root, int n, const vector<int64>& cost) {
    vector<int64> weights;
    auto read_row = [&](int base) {
        weights.clear();
        for (int value = base; value <= n; value *= 2) {
            weights.push_back(cost[value]);
        }
    };

    int base = root;
    read_row(base);
    int length = static_cast<int>(weights.size());
    vector<int64> dp(1 << length, 0), next_dp, best;

    // A set bit means that the corresponding number is removed.
    for (int mask = 1; mask < static_cast<int>(dp.size()); ++mask) {
        dp[mask] = dp[mask & (mask - 1)] + weights[__builtin_ctz(mask)];
    }

    constexpr int64 INF = numeric_limits<int64>::max() / 4;
    while (length >= 3) {
        const int states = 1 << (length - 2);
        const int full = states - 1;
        best.assign(states, INF);

        for (int mask = 0; mask < static_cast<int>(dp.size()); ++mask) {
            const int kept = (static_cast<int>(dp.size()) - 1) ^ mask;
            // Keeping k, 2k, and 4k requires removing 3k in the next row.
            const int required = kept & (kept >> 1) & (kept >> 2);
            best[required] = min(best[required], dp[mask]);
        }

        // best[mask] becomes the minimum over all subsets of mask.
        for (int bit = 1; bit < states; bit <<= 1) {
            for (int start = 0; start < states; start += bit << 1) {
                for (int offset = 0; offset < bit; ++offset) {
                    best[start + bit + offset] = min(
                        best[start + bit + offset], best[start + offset]);
                }
            }
        }

        base *= 3;
        read_row(base);
        length = static_cast<int>(weights.size());
        next_dp.resize(1 << length);
        next_dp[0] = 0;
        for (int mask = 1; mask < static_cast<int>(next_dp.size()); ++mask) {
            next_dp[mask] = next_dp[mask & (mask - 1)]
                          + weights[__builtin_ctz(mask)];
        }
        for (int mask = 0; mask < static_cast<int>(next_dp.size()); ++mask) {
            next_dp[mask] += best[mask & full];
        }
        dp.swap(next_dp);
    }

    // Later rows have fewer than three numbers and create no new constraints.
    return *min_element(dp.begin(), dp.end());
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<int64> cost(n + 1);
    for (int i = 1; i <= n; ++i) cin >> cost[i];

    int64 answer = 0;
    for (int root = 1; root <= n / 4; ++root) {
        if (root % 2 != 0 && root % 3 != 0) {
            answer += solve_component(root, n, cost);
        }
    }
    cout << answer << '\n';
    return 0;
}
