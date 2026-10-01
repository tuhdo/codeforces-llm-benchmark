#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

static int64 ceil_div_nonnegative(int64 value, int64 divisor) {
    return value / divisor + (value % divisor != 0);
}

static int64 solve_one_dimensional(const vector<int64>& values, int k) {
    const int m = static_cast<int>(values.size());

    if (m == 1) {
        return values[0] >= 0 ? 0 : -1;
    }

    if (m == 2) {
        if (k == 1) return 0;
        return llabs(values[0] - values[1]);
    }

    const int64 total = accumulate(values.begin(), values.end(), int64{0});
    vector<int64> deficit(m);
    for (int i = 0; i < m; ++i) {
        deficit[i] = max<int64>(0, total - 2 * values[i]);
    }

    const int64 baseline_gain = m - 2;
    vector<int64> interior_deficit(deficit.begin() + 1, deficit.end() - 1);
    sort(interior_deficit.begin(), interior_deficit.end());
    const int interior_count = m - 2;

    auto feasible = [&](int64 operations) {
        const int64 baseline = baseline_gain * operations;

        auto special_cap = [&](int required_interiors) -> int64 {
            if (required_interiors < 0 || required_interiors > interior_count) {
                return -1;
            }
            if (required_interiors == 0) return operations;

            const int64 required = interior_deficit[required_interiors - 1];
            if (baseline < required) return -1;
            return min(operations, baseline - required);
        };

        // Count only interior cells.
        if (k <= interior_count) {
            const int64 cap = special_cap(k);
            if (cap >= 0) return true; // q = 0 uses no special intervals.
        }

        // Count one endpoint and k - 1 interior cells.
        const int needed_inside_one_endpoint = k - 1;
        const int64 cap_one_endpoint = special_cap(needed_inside_one_endpoint);
        if (cap_one_endpoint >= 0) {
            const int64 need_left = max<int64>(0, deficit.front() - baseline);
            if (need_left <= cap_one_endpoint) return true;

            const int64 need_right = max<int64>(0, deficit.back() - baseline);
            if (need_right <= cap_one_endpoint) return true;
        }

        // Count both endpoints and k - 2 interior cells. Let q be the
        // difference between intervals omitting the left and right endpoint.
        const int needed_inside_both_endpoints = k - 2;
        const int64 cap_both_endpoints = special_cap(needed_inside_both_endpoints);
        if (cap_both_endpoints >= 0) {
            const int64 low = deficit.front() - baseline;
            const int64 high = baseline - deficit.back();
            if (max(low, -cap_both_endpoints) <= min(high, cap_both_endpoints)) {
                return true;
            }
        }

        return false;
    };

    int64 high = 0;
    for (int64 d : deficit) {
        high = max(high, ceil_div_nonnegative(d, baseline_gain));
    }
    int64 low = 0;
    while (low < high) {
        const int64 mid = low + (high - low) / 2;
        if (feasible(mid)) high = mid;
        else low = mid + 1;
    }
    return low;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n, m, k;
        cin >> n >> m >> k;

        vector<vector<int64>> a(n, vector<int64>(m));
        for (auto& row : a) {
            for (int64& value : row) cin >> value;
        }

        int64 answer;
        if (n == 1 || m == 1) {
            vector<int64> values;
            if (n == 1) {
                values = move(a[0]);
            } else {
                values.reserve(n);
                for (int i = 0; i < n; ++i) values.push_back(a[i][0]);
            }
            answer = solve_one_dimensional(values, k);
        } else {
            vector<int64> row_sum(n, 0), column_sum(m, 0);
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    row_sum[i] += a[i][j];
                    column_sum[j] += a[i][j];
                }
            }

            vector<int64> deficit;
            deficit.reserve(static_cast<size_t>(n) * m);
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    deficit.push_back(max<int64>(0, row_sum[i] + column_sum[j] - 3 * a[i][j]));
                }
            }
            sort(deficit.begin(), deficit.end());
            const int64 kth_deficit = deficit[k - 1];
            const int64 gain_per_operation = n + m - 3;
            answer = ceil_div_nonnegative(kth_deficit, gain_per_operation);
        }

        cout << answer << '\n';
    }
    return 0;
}
