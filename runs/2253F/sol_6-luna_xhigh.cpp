#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
static constexpr int64 INF = (1LL << 62);

static int64 solve_component(int n, int t, const vector<int64>& removal_cost) {
    vector<vector<int64>> column_costs;

    for (int64 base = t; base <= n; base *= 2) {
        vector<int> values;
        for (int64 value = base; value <= n; value *= 3) {
            values.push_back(static_cast<int>(value));
        }

        const int count = 1 << values.size();
        vector<int64> sums(count, 0);
        for (int mask = 1; mask < count; ++mask) {
            const int bit = __builtin_ctz(static_cast<unsigned>(mask));
            sums[mask] = sums[mask & (mask - 1)] + removal_cost[values[bit]];
        }
        column_costs.push_back(move(sums));
    }

    const int columns = static_cast<int>(column_costs.size());
    if (columns < 3) return 0;

    int count_p = static_cast<int>(column_costs[0].size());
    int count_q = static_cast<int>(column_costs[1].size());
    vector<int64> dp(static_cast<size_t>(count_p) * count_q);

    // dp[q][p] is the cost of selecting masks p and q in the first two columns.
    for (int q = 0; q < count_q; ++q) {
        for (int p = 0; p < count_p; ++p) {
            dp[static_cast<size_t>(q) * count_p + p] =
                column_costs[1][q] + column_costs[0][p];
        }
    }

    for (int c = 0; c + 2 < columns; ++c) {
        const int count_r = static_cast<int>(column_costs[c + 2].size());
        const int rows_p = __builtin_ctz(static_cast<unsigned>(count_p));
        const int rows_q = __builtin_ctz(static_cast<unsigned>(count_q));
        const int rows_r = __builtin_ctz(static_cast<unsigned>(count_r));

        const int active_rows = min(rows_r, max(0, rows_p - 1));
        const int active_mask = active_rows == 0 ? 0 : (1 << active_rows) - 1;
        const int mask_p = count_p - 1;
        const int mask_q = count_q - 1;

        vector<int> adjacent_zero(count_p);
        for (int p = 0; p < count_p; ++p) {
            const int zero = (~p) & mask_p;
            adjacent_zero[p] = zero & (zero >> 1) & active_mask;
        }

        vector<int64> next_dp(static_cast<size_t>(count_q) * count_r, INF);
        vector<int64> best(count_r);

        for (int q = 0; q < count_q; ++q) {
            fill(best.begin(), best.end(), INF);
            const int q_zero = (~q) & mask_q;
            const int64* previous = dp.data() + static_cast<size_t>(q) * count_p;

            for (int p = 0; p < count_p; ++p) {
                const int required = adjacent_zero[p] & q_zero;
                best[required] = min(best[required], previous[p]);
            }

            // For a current-column mask r, all constraints are met if it
            // contains every bit required by the previous two columns.
            for (int bit = 0; bit < rows_r; ++bit) {
                const int bit_mask = 1 << bit;
                for (int mask = 0; mask < count_r; ++mask) {
                    if (mask & bit_mask) {
                        best[mask] = min(best[mask], best[mask ^ bit_mask]);
                    }
                }
            }

            int64* current = next_dp.data() + static_cast<size_t>(q) * count_r;
            for (int r = 0; r < count_r; ++r) {
                current[r] = best[r] + column_costs[c + 2][r];
            }
        }

        dp.swap(next_dp);
        count_p = count_q;
        count_q = count_r;
    }

    return *min_element(dp.begin(), dp.end());
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    vector<int64> removal_cost(n + 1);
    for (int i = 1; i <= n; ++i) cin >> removal_cost[i];

    int64 answer = 0;
    for (int t = 1; t <= n / 4; ++t) {
        if (t % 2 == 0 || t % 3 == 0) continue;
        answer += solve_component(n, t, removal_cost);
    }

    cout << answer << '\n';
    return 0;
}
