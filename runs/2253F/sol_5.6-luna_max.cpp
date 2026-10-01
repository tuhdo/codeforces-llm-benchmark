#include <bits/stdc++.h>

using namespace std;

using int64 = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) {
        return 0;
    }

    vector<int64> remove_cost(n + 1);
    for (int i = 1; i <= n; ++i) {
        cin >> remove_cost[i];
    }

    const int last_start = n / 4;
    const int64 INF = (1LL << 62);
    int64 answer = 0;

    // Every quadruple {a, 2a, 3a, 4a} stays inside one component whose
    // numbers have the same part after removing all factors 2 and 3.
    for (int core = 1; core <= last_start; ++core) {
        if (core % 2 == 0 || core % 3 == 0) {
            continue;
        }

        int64 row_base = core;
        vector<int64> dp;
        int previous_active = -1;

        while (row_base <= n) {
            int width = 0;
            for (int64 value = row_base; value <= n; value *= 2) {
                ++width;
                if (value > n / 2) {
                    break;
                }
            }

            vector<int64> values(width);
            int64 value = row_base;
            for (int bit = 0; bit < width; ++bit) {
                values[bit] = value;
                value *= 2;
            }

            const int states = 1 << width;
            vector<int64> row_cost(states, 0);
            for (int mask = 1; mask < states; ++mask) {
                const int bit = __builtin_ctz(static_cast<unsigned>(mask));
                row_cost[mask] = row_cost[mask ^ (1 << bit)] + remove_cost[values[bit]];
            }

            int active = 0;
            if (row_base <= last_start) {
                int64 start = row_base;
                while (start <= last_start) {
                    ++active;
                    start *= 2;
                }
            }

            if (dp.empty()) {
                dp = move(row_cost);
            } else {
                const int previous_states = static_cast<int>(dp.size());
                const int next_states = states;
                const int forced_mask_limit = (1 << previous_active) - 1;

                vector<int64> best(next_states, INF);
                for (int mask = 0; mask < previous_states; ++mask) {
                    // A zero triple in the previous row forces the same
                    // column in the current row to be removed.
                    const int covered = mask | (mask >> 1) | (mask >> 2);
                    const int forced = (~covered) & forced_mask_limit;
                    best[forced] = min(best[forced], dp[mask]);
                }

                // best[mask] = min(best[submask]) over all submasks.
                for (int bit = 1; bit < next_states; bit <<= 1) {
                    for (int mask = 0; mask < next_states; ++mask) {
                        if (mask & bit) {
                            best[mask] = min(best[mask], best[mask ^ bit]);
                        }
                    }
                }

                vector<int64> next_dp(next_states);
                for (int mask = 0; mask < next_states; ++mask) {
                    next_dp[mask] = best[mask] + row_cost[mask];
                }
                dp = move(next_dp);
            }

            if (active == 0) {
                break;
            }

            previous_active = active;
            row_base *= 3;
        }

        answer += *min_element(dp.begin(), dp.end());
    }

    cout << answer << '\n';
    return 0;
}
