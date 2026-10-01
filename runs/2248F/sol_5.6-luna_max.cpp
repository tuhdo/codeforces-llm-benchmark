#include <bits/stdc++.h>

using namespace std;

using int64 = long long;
using i128 = __int128_t;

static bool feasible_1d(const vector<int64>& margin, int need, int64 operations) {
    const int n = static_cast<int>(margin.size());
    vector<int64> eligible;
    eligible.reserve(n);

    for (int i = 0; i < n; ++i) {
        const int64 deficit = -margin[i];
        const int64 best_per_operation = max<int64>({
            n - 2,
            i,
            n - 1 - i,
        });

        if (static_cast<i128>(deficit) <=
            static_cast<i128>(operations) * best_per_operation) {
            eligible.push_back(deficit);
        }
    }

    if (static_cast<int>(eligible.size()) < need) {
        return false;
    }
    if (need == 1) {
        return true;
    }

    nth_element(eligible.begin(), eligible.begin() + (need - 1), eligible.end());
    const int64 largest_of_selected = eligible[need - 1];
    const int64 second_largest_of_selected =
        *max_element(eligible.begin(), eligible.begin() + (need - 1));

    return static_cast<i128>(largest_of_selected) + second_largest_of_selected <=
           static_cast<i128>(2) * operations * (n - 2);
}

static int64 solve_1d(const vector<int64>& margin, int need) {
    const int n = static_cast<int>(margin.size());

    if (n == 1) {
        return margin[0] >= 0 ? 0 : -1;
    }

    int64 high = 0;
    if (n == 2) {
        for (int64 value : margin) {
            high = max(high, -value);
        }
    } else {
        const int64 per_operation = n - 2;
        for (int64 value : margin) {
            if (value < 0) {
                const int64 deficit = -value;
                high = max(high, (deficit + per_operation - 1) / per_operation);
            }
        }
    }

    if (feasible_1d(margin, need, 0)) {
        return 0;
    }
    if (!feasible_1d(margin, need, high)) {
        // This cannot occur for a valid one-dimensional instance with n >= 2,
        // but keep the failure mode explicit.
        return -1;
    }

    int64 low = 0;
    while (high - low > 1) {
        const int64 middle = low + (high - low) / 2;
        if (feasible_1d(margin, need, middle)) {
            high = middle;
        } else {
            low = middle;
        }
    }
    return high;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int test_cases;
    cin >> test_cases;
    while (test_cases--) {
        int n, m, k;
        cin >> n >> m >> k;

        vector<vector<int64>> value(n, vector<int64>(m));
        vector<int64> row_sum(n, 0), column_sum(m, 0);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                cin >> value[i][j];
                row_sum[i] += value[i][j];
                column_sum[j] += value[i][j];
            }
        }

        vector<int64> margin;
        margin.reserve(static_cast<size_t>(n) * m);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                margin.push_back(3 * value[i][j] - row_sum[i] - column_sum[j]);
            }
        }

        int64 answer;
        if (n == 1 || m == 1) {
            vector<int64> line;
            line.reserve(max(n, m));
            if (n == 1) {
                line = margin;
            } else {
                for (int i = 0; i < n; ++i) {
                    line.push_back(margin[i]);
                }
            }
            answer = solve_1d(line, k);
        } else {
            sort(margin.begin(), margin.end(), greater<int64>());
            const int64 kth_margin = margin[k - 1];
            if (kth_margin >= 0) {
                answer = 0;
            } else {
                const int64 per_operation = n + m - 3;
                answer = static_cast<int64>(
                    (-(static_cast<i128>(kth_margin)) + per_operation - 1) /
                    per_operation);
            }
        }

        cout << answer << '\n';
    }
    return 0;
}
