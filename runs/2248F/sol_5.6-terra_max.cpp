#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

using namespace std;

using int64 = long long;

static int64 ceil_div_positive(int64 x, int64 y) {
    // x is positive and y is positive.
    return (x + y - 1) / y;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n, m, k;
        cin >> n >> m >> k;

        vector<vector<int64>> v(n, vector<int64>(m));
        vector<int64> row(n, 0), col(m, 0);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                cin >> v[i][j];
                row[i] += v[i][j];
                col[j] += v[i][j];
            }
        }

        if (n >= 2 && m >= 2) {
            vector<int64> score;
            score.reserve(n * m);
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    // This is non-negative exactly when (i, j) is a peak.
                    score.push_back(3 * v[i][j] - row[i] - col[j]);
                }
            }
            nth_element(score.begin(), score.begin() + (k - 1), score.end(), greater<int64>());
            int64 kth_score = score[k - 1];
            int64 gain = n + m - 3;
            cout << (kth_score >= 0 ? 0 : ceil_div_positive(-kth_score, gain)) << '\n';
            continue;
        }

        // A one-dimensional matrix is an array.  Its only nontrivial
        // operations are decrements on contiguous subarrays.
        vector<int64> a;
        if (n == 1) {
            a = v[0];
        } else {
            a.resize(n);
            for (int i = 0; i < n; ++i) {
                a[i] = v[i][0];
            }
        }

        int N = static_cast<int>(a.size());
        if (N == 1) {
            cout << (a[0] >= 0 ? 0 : -1) << '\n';
            continue;
        }
        if (N == 2) {
            // One peak is always present.  Both are peaks iff the values match.
            cout << (k == 1 ? 0 : llabs(a[0] - a[1])) << '\n';
            continue;
        }

        int64 sum = 0;
        for (int64 x : a) {
            sum += x;
        }

        // score[i] = 2*a[i]-sum is the peak inequality for an array.
        vector<int64> score(N);
        for (int i = 0; i < N; ++i) {
            score[i] = 2 * a[i] - sum;
        }

        vector<int64> middle;
        middle.reserve(N - 2);
        for (int i = 1; i + 1 < N; ++i) {
            middle.push_back(score[i]);
        }
        sort(middle.begin(), middle.end());

        const int64 full_gain = N - 2;
        const int64 special_base_gain = N - 3;

        auto middle_count = [&](int64 z) -> int {
            // Count middle scores that become non-negative after gain z.
            return static_cast<int>(middle.end() -
                                    lower_bound(middle.begin(), middle.end(), -z));
        };

        auto endpoint_need = [](int64 initial_score, int64 z) -> int64 {
            // Number of length-(N-1) operations which exclude this endpoint.
            int64 missing = -initial_score - z;
            return missing <= 0 ? 0 : (missing + 1) / 2;
        };

        auto smallest_z_for_middle = [&](int required, int64 base, int64 total,
                                         int64& z) -> bool {
            z = base;
            if (required > 0) {
                if (required > static_cast<int>(middle.size())) {
                    return false;
                }
                // required-th largest middle score.
                int64 threshold_score = middle[middle.size() - required];
                z = max(z, -threshold_score);
            }
            return z <= total;
        };

        auto feasible = [&](int64 operations) -> bool {
            // Every useful operation can be replaced by one of:
            // [1,N], [1,N-1], [2,N].  Let z be the gain of a middle cell.
            int64 total = full_gain * operations;
            int64 base = special_base_gain * operations;

            // All required peaks can be middle cells.
            if (middle_count(total) >= k) {
                return true;
            }

            // Exactly one endpoint is needed.
            int64 z;
            if (smallest_z_for_middle(k - 1, base, total, z)) {
                int64 available_special = total - z;
                int64 need_left = endpoint_need(score[0], z);
                int64 need_right = endpoint_need(score[N - 1], z);
                if (min(need_left, need_right) <= available_special) {
                    return true;
                }
            }

            // Both endpoints are needed.  For a fixed parity, the required
            // amount is unchanged while both endpoints still need help, so
            // the two smallest possible z values cover all optima.
            if (smallest_z_for_middle(k - 2, base, total, z)) {
                for (int add = 0; add <= 1; ++add) {
                    int64 candidate = z + add;
                    if (candidate > total) {
                        continue;
                    }
                    int64 available_special = total - candidate;
                    int64 need_left = endpoint_need(score[0], candidate);
                    int64 need_right = endpoint_need(score[N - 1], candidate);
                    if (need_left + need_right <= available_special) {
                        return true;
                    }
                }
            }

            return false;
        };

        // Repeating the full-array operation makes every score grow by N-2,
        // so this is always a valid upper bound for N >= 3.
        int64 high = 0;
        for (int64 x : score) {
            if (x < 0) {
                high = max(high, ceil_div_positive(-x, full_gain));
            }
        }

        int64 low = 0;
        while (low < high) {
            int64 mid = low + (high - low) / 2;
            if (feasible(mid)) {
                high = mid;
            } else {
                low = mid + 1;
            }
        }
        cout << low << '\n';
    }
    return 0;
}
