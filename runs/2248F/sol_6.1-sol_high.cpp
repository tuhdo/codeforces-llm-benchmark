#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

using namespace std;
using int64 = long long;

static int64 ceil_positive(int64 value, int64 divisor) {
    return value <= 0 ? 0 : (value + divisor - 1) / divisor;
}

static int64 solve_line(const vector<int64>& values, int k) {
    const int length = static_cast<int>(values.size());
    if (length == 1) return values[0] >= 0 ? 0 : -1;
    if (length == 2) return k == 1 ? 0 : llabs(values[0] - values[1]);

    int64 sum = 0;
    for (int64 value : values) sum += value;
    const int64 first = sum - 2 * values.front();
    const int64 last = sum - 2 * values.back();
    const int64 d = length - 2;

    vector<int64> interior;
    for (int i = 1; i + 1 < length; ++i) {
        interior.push_back(sum - 2 * values[i]);
    }
    sort(interior.begin(), interior.end());

    // Full intervals dominate every operation except the two intervals
    // that contain all elements except one endpoint.
    int64 answer = numeric_limits<int64>::max();
    for (int mask = 0; mask < 4; ++mask) {
        const bool take_first = (mask & 1) != 0;
        const bool take_last = (mask & 2) != 0;
        const int needed = k - static_cast<int>(take_first) - static_cast<int>(take_last);
        if (needed < 0 || needed > static_cast<int>(interior.size())) continue;

        int64 operations = 0;
        const int64 threshold = needed > 0 ? interior[needed - 1] : 0;
        if (needed > 0) operations = ceil_positive(threshold, d);

        // With t operations and z = (exclude-first) - (exclude-last),
        // endpoints improve by d*t +/- z. Interior cells improve by
        // d*t - |z| when the remaining operations use the full interval.
        // The following bounds are exactly the conditions for such a z.
        if (take_first) {
            operations = max(operations, ceil_positive(first, d + 1));
            if (needed > 0) {
                operations = max(operations, ceil_positive(first + threshold, 2 * d));
            }
        }
        if (take_last) {
            operations = max(operations, ceil_positive(last, d + 1));
            if (needed > 0) {
                operations = max(operations, ceil_positive(last + threshold, 2 * d));
            }
        }
        if (take_first && take_last) {
            operations = max(operations, ceil_positive(first + last, 2 * d));
        }
        answer = min(answer, operations);
    }
    return answer;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n, m, k;
        cin >> n >> m >> k;
        vector<int64> values(n * m), rows(n, 0), columns(m, 0);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                int64& value = values[i * m + j];
                cin >> value;
                rows[i] += value;
                columns[j] += value;
            }
        }

        if (n == 1 || m == 1) {
            cout << solve_line(values, k) << '\n';
            continue;
        }

        // A full-matrix operation reduces every peak deficit by n+m-3,
        // the largest reduction any rectangle can give any cell.
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                values[i * m + j] = rows[i] + columns[j] - 3 * values[i * m + j];
            }
        }
        nth_element(values.begin(), values.begin() + k - 1, values.end());
        cout << ceil_positive(values[k - 1], n + m - 3) << '\n';
    }
}
