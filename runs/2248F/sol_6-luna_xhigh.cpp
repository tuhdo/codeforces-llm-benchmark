#include <bits/stdc++.h>
using namespace std;

static bool feasible_line(long long operations, long long length,
                          long long first_score, long long last_score,
                          const vector<long long>& sorted_interior, int required) {
    const long long step = length - 2;
    const long long base = operations * step;
    const long long first_limit = first_score + base;
    const long long last_limit = -last_score - base;

    auto endpoint_max = [&](long long left, long long right) {
        const bool first_possible = right >= max(left, -first_limit);
        const bool last_possible = right >= max(left, last_limit);

        bool both_possible = false;
        if (first_limit >= last_limit) {
            if (left < right) {
                const long long z_left = max(last_limit, -right);
                const long long z_right = min(first_limit, right);
                both_possible = z_left <= z_right;
            } else {
                const long long z_left = max(last_limit, -left);
                const long long z_right = min(first_limit, left);
                if (z_left <= z_right) {
                    long long candidate = z_left;
                    if ((candidate % 2 + 2) % 2 != left % 2) {
                        ++candidate;
                    }
                    both_possible = candidate <= z_right;
                }
            }
        }

        if (both_possible) return 2;
        return (first_possible || last_possible) ? 1 : 0;
    };

    auto interval_works = [&](long long left, long long right, int interior_count) {
        if (left > right) return false;
        return interior_count + endpoint_max(left, right) >= required;
    };

    // For an interior position i, its score after s special operations is
    // peaks[i] + base - s. It remains a peak exactly while
    // peaks[i] + base >= s. The count is constant between these events.
    const int interior_end = static_cast<int>(sorted_interior.size());
    int pos = static_cast<int>(lower_bound(sorted_interior.begin(), sorted_interior.end(),
                                           -base) - sorted_interior.begin());
    int interior_count = interior_end - pos;

    long long left = 0;
    while (pos < interior_end && sorted_interior[pos] + base < operations) {
        const long long value = sorted_interior[pos];
        const long long event = value + base + 1;
        if (interval_works(left, event - 1, interior_count)) return true;

        int next = pos;
        while (next < interior_end && sorted_interior[next] == value) ++next;
        interior_count -= next - pos;
        pos = next;
        left = event;
    }

    return interval_works(left, operations, interior_count);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int test_cases;
    cin >> test_cases;
    while (test_cases--) {
        int n, m, k;
        cin >> n >> m >> k;

        vector<vector<long long>> values(n, vector<long long>(m));
        vector<long long> row_sum(n, 0), column_sum(m, 0);
        long long total = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                cin >> values[i][j];
                row_sum[i] += values[i][j];
                column_sum[j] += values[i][j];
                total += values[i][j];
            }
        }

        if (n == 1 || m == 1) {
            const int length = max(n, m);
            vector<long long> peak(length);
            for (int i = 0; i < length; ++i) {
                const long long value = (n == 1) ? values[0][i] : values[i][0];
                peak[i] = 2 * value - total;
            }

            if (length == 1) {
                cout << (peak[0] >= 0 ? 0 : -1) << '\n';
                continue;
            }

            if (length == 2) {
                if (k == 1) cout << 0 << '\n';
                else cout << llabs(peak[0]) << '\n';
                continue;
            }

            vector<long long> sorted_interior(peak.begin() + 1, peak.end() - 1);
            sort(sorted_interior.begin(), sorted_interior.end());

            long long high = 0;
            const long long step = length - 2;
            for (long long score : peak) {
                if (score < 0) {
                    high = max(high, (-score + step - 1) / step);
                }
            }

            long long low = 0;
            while (low < high) {
                const long long mid = low + (high - low) / 2;
                if (feasible_line(mid, length, peak.front(), peak.back(),
                                  sorted_interior, k)) high = mid;
                else low = mid + 1;
            }
            cout << low << '\n';
            continue;
        }

        vector<long long> peak;
        peak.reserve(static_cast<size_t>(n) * m);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                peak.push_back(3 * values[i][j] - row_sum[i] - column_sum[j]);
            }
        }

        const long long step = static_cast<long long>(n) + m - 3;
        nth_element(peak.begin(), peak.begin() + (k - 1), peak.end(), greater<long long>());
        const long long kth_largest = peak[k - 1];
        long long answer = 0;
        if (kth_largest < 0) {
            answer = (-kth_largest + step - 1) / step;
        } else {
            answer = 0;
        }
        cout << answer << '\n';
    }
    return 0;
}
