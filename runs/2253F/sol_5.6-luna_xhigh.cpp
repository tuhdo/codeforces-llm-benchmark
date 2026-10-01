#include <bits/stdc++.h>

using namespace std;

using int64 = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    vector<int64> cost(n + 1);
    for (int i = 1; i <= n; ++i) {
        cin >> cost[i];
    }

    constexpr int64 INF = (1LL << 62);
    int64 answer = 0;

    // Every number has a unique form b * 2^p * 3^q, with gcd(b, 6) = 1.
    // A quadruple {a, 2a, 3a, 4a} stays inside one such component.
    for (int b = 1; 4 * b <= n; ++b) {
        if (b % 2 == 0 || b % 3 == 0) {
            continue;
        }

        vector<int> row_value;
        vector<int> width;
        for (long long value = b; value <= n; value *= 3) {
            row_value.push_back(static_cast<int>(value));

            int w = 0;
            for (long long x = value; x <= n; x *= 2) {
                ++w;
            }
            width.push_back(w);
        }

        int rows = static_cast<int>(row_value.size());

        auto row_costs = [&](int row) {
            int w = width[row];
            vector<int64> cell_cost(w);
            long long value = row_value[row];
            for (int p = 0; p < w; ++p) {
                cell_cost[p] = cost[static_cast<int>(value)];
                value *= 2;
            }

            vector<int64> result(1u << w, 0);
            for (unsigned mask = 1; mask < result.size(); ++mask) {
                unsigned bit = mask & -mask;
                int p = __builtin_ctz(bit);
                result[mask] = result[mask ^ bit] + cell_cost[p];
            }
            return result;
        };

        // dp[mask] is the minimum cost in rows q..top when row q has mask.
        vector<int64> dp = row_costs(rows - 1);

        for (int q = rows - 2; q >= 0; --q) {
            int current_width = width[q];
            int next_width = width[q + 1];

            // For every required subset R of the next row, find the cheapest
            // next-row mask containing R.
            vector<int64> superset_min = dp;
            for (int bit = 0; bit < next_width; ++bit) {
                unsigned step = 1u << bit;
                for (unsigned mask = 0; mask < superset_min.size(); ++mask) {
                    if ((mask & step) == 0) {
                        superset_min[mask] = min(superset_min[mask],
                                                 superset_min[mask | step]);
                    }
                }
            }

            vector<int64> current_cost = row_costs(q);
            vector<int64> next_dp(1u << current_width, INF);
            unsigned current_all = (1u << current_width) - 1;
            unsigned next_all = (1u << next_width) - 1;

            for (unsigned mask = 0; mask < next_dp.size(); ++mask) {
                // If the three horizontal cells at p are all kept, then the
                // cell at (p, q+1) must be removed.
                unsigned kept = (~mask) & current_all;
                unsigned required = kept & (kept >> 1) & (kept >> 2);

                if ((required & ~next_all) == 0) {
                    next_dp[mask] = current_cost[mask] +
                                    superset_min[required];
                }
            }

            dp.swap(next_dp);
        }

        answer += *min_element(dp.begin(), dp.end());
    }

    cout << answer << '\n';
    return 0;
}
