#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

using int64 = long long;
using i128 = __int128_t;

static int64 solve_one_dim(const vector<int64>& a, int k) {
    const int len = static_cast<int>(a.size());

    if (len == 1) {
        return a[0] >= 0 ? 0 : -1;
    }

    if (len == 2) {
        if (k == 1) return 0;
        return llabs(a[0] - a[1]);
    }

    const int64 sum = accumulate(a.begin(), a.end(), int64(0));
    vector<int64> score(len);
    for (int i = 0; i < len; ++i) score[i] = 2 * a[i] - sum;

    const int64 bonus = len - 2;
    vector<int64> interior(score.begin() + 1, score.end() - 1);
    sort(interior.begin(), interior.end(), greater<int64>());

    auto feasible = [&](int64 operations) {
        const i128 base = static_cast<i128>(bonus) * operations;
        const i128 upper_operations = operations;
        const i128 left_need = -static_cast<i128>(score.front()) - base;
        const i128 right_limit = static_cast<i128>(score.back()) + base;

        // t is the number of endpoint-focused operations.  The remaining
        // operations are whole-array operations.
        for (int endpoints = 0; endpoints <= 2; ++endpoints) {
            const int needed_interior = k - endpoints;
            if (needed_interior < 0 || needed_interior > len - 2) continue;

            i128 upper_t = upper_operations;
            if (needed_interior > 0) {
                upper_t = min(upper_t,
                              static_cast<i128>(interior[needed_interior - 1]) + base);
            }
            if (upper_t < 0) continue;

            if (endpoints == 0) {
                return true;
            }

            if (endpoints == 1) {
                const i128 left_t = max<i128>(0, left_need);
                const i128 right_t = max<i128>(0, -right_limit);
                if (min(left_t, right_t) <= upper_t) return true;
                continue;
            }

            // If u is the difference between left-focused and right-focused
            // operations, both endpoints require left_need <= u <= right_limit.
            // We may choose t = |u|, so only the reachable range [-upper_t, upper_t]
            // matters.
            if (left_need <= right_limit) {
                const i128 low = max(left_need, -upper_t);
                const i128 high = min(right_limit, upper_t);
                if (low <= high) return true;
            }
        }
        return false;
    };

    int64 upper = 0;
    for (int64 x : score) {
        if (x < 0) {
            upper = max(upper, static_cast<int64>((-static_cast<i128>(x) + bonus - 1) / bonus));
        }
    }

    int64 low = 0;
    while (low < upper) {
        const int64 mid = low + (upper - low) / 2;
        if (feasible(mid)) upper = mid;
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

        vector<int64> values(static_cast<size_t>(n) * m);
        for (auto& x : values) cin >> x;

        if (n == 1 || m == 1) {
            if (n == 1) {
                cout << solve_one_dim(values, k) << '\n';
            } else {
                vector<int64> column(n);
                for (int i = 0; i < n; ++i) column[i] = values[i];
                cout << solve_one_dim(column, k) << '\n';
            }
            continue;
        }

        vector<int64> row_sum(n, 0), col_sum(m, 0);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                const int64 x = values[static_cast<size_t>(i) * m + j];
                row_sum[i] += x;
                col_sum[j] += x;
            }
        }

        vector<int64> scores;
        scores.reserve(values.size());
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                const int64 x = values[static_cast<size_t>(i) * m + j];
                scores.push_back(3 * x - row_sum[i] - col_sum[j]);
            }
        }

        nth_element(scores.begin(), scores.begin() + (k - 1), scores.end(), greater<int64>());
        const int64 kth_score = scores[k - 1];
        const int64 per_operation = n + m - 3;
        const int64 answer = kth_score >= 0
                                 ? 0
                                 : static_cast<int64>((-static_cast<i128>(kth_score) + per_operation - 1) /
                                                      per_operation);
        cout << answer << '\n';
    }
}
