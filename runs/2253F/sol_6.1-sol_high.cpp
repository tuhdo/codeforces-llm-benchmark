#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<int64> cost(n + 1);
    for (int i = 1; i <= n; ++i) cin >> cost[i];

    const int64 INF = numeric_limits<int64>::max() / 4;
    int64 answer = 0;

    vector<int64> weights, dp, next_dp, sums;
    for (int root = 1; root <= n / 4; ++root) {
        if (root % 2 == 0 || root % 3 == 0) continue;

        weights.clear();
        for (int value = root; value <= n; value *= 2) {
            weights.push_back(cost[value]);
        }
        int width = static_cast<int>(weights.size());
        int states = 1 << width;
        dp.resize(states);
        dp[0] = 0;
        for (int mask = 1; mask < states; ++mask) {
            int bit = __builtin_ctz(static_cast<unsigned>(mask));
            dp[mask] = dp[mask & (mask - 1)] + weights[bit];
        }

        int row_base = root;
        while (width >= 3) {
            row_base *= 3;
            weights.clear();
            for (int value = row_base; value <= n; value *= 2) {
                weights.push_back(cost[value]);
            }
            int next_width = static_cast<int>(weights.size());
            int next_states = 1 << next_width;
            next_dp.assign(next_states, INF);

            // Three kept consecutive cells force removal of the cell above.
            for (int mask = 0; mask < states; ++mask) {
                int kept = (states - 1) ^ mask;
                int forced = kept & (kept >> 1) & (kept >> 2);
                next_dp[forced] = min(next_dp[forced], dp[mask]);
            }

            // For each removal mask, minimize over all contained forced masks.
            for (int bit = 1; bit < next_states; bit <<= 1) {
                for (int start = 0; start < next_states; start += bit * 2) {
                    for (int offset = 0; offset < bit; ++offset) {
                        int mask = start + bit + offset;
                        next_dp[mask] = min(next_dp[mask], next_dp[mask - bit]);
                    }
                }
            }

            sums.resize(next_states);
            sums[0] = 0;
            for (int mask = 1; mask < next_states; ++mask) {
                int bit = __builtin_ctz(static_cast<unsigned>(mask));
                sums[mask] = sums[mask & (mask - 1)] + weights[bit];
            }
            for (int mask = 0; mask < next_states; ++mask) {
                next_dp[mask] += sums[mask];
            }

            dp.swap(next_dp);
            width = next_width;
            states = next_states;
        }

        answer += *min_element(dp.begin(), dp.end());
    }

    cout << answer << '\n';
    return 0;
}
