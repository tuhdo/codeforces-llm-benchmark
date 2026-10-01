#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int64 ceil_nonnegative(int64 value, int64 divisor) {
    return value <= 0 ? 0 : (value + divisor - 1) / divisor;
}

int64 solve_line(const vector<int64>& values, int k) {
    const int length = static_cast<int>(values.size());
    if (length == 1) {
        return values[0] >= 0 ? 0 : -1;
    }
    if (length == 2) {
        return k == 1 ? 0 : abs(values[0] - values[1]);
    }

    const int64 sum = accumulate(values.begin(), values.end(), 0LL);
    const int64 first = sum - 2 * values.front();
    const int64 last = sum - 2 * values.back();
    vector<int64> middle;
    middle.reserve(length - 2);
    for (int i = 1; i + 1 < length; ++i) {
        middle.push_back(sum - 2 * values[i]);
    }
    sort(middle.begin(), middle.end());

    const int64 rate = length - 2;
    int64 answer = LLONG_MAX;
    for (unsigned mask = 0; mask < 4; ++mask) {
        const int count = k - popcount(mask);
        if (count < 0 || count > length - 2) {
            continue;
        }

        const int64 threshold = count > 0 ? middle[count - 1] : 0;
        int64 operations = count > 0
            ? ceil_nonnegative(threshold, rate)
            : 0;

        // An endpoint can gain at most rate + 1 per operation.
        // Its gain plus any middle cell's gain is at most 2 * rate.
        for (int endpoint = 0; endpoint < 2; ++endpoint) {
            if ((mask & (1U << endpoint)) == 0) {
                continue;
            }
            const int64 deficit = endpoint == 0 ? first : last;
            operations = max(operations,
                             ceil_nonnegative(deficit, rate + 1));
            if (count > 0) {
                operations = max(operations,
                                 ceil_nonnegative(deficit + threshold,
                                                  2 * rate));
            }
        }
        if (mask == 3) {
            operations = max(operations,
                             ceil_nonnegative(first + last, 2 * rate));
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
        vector<int64> values(n * m);
        vector<int64> row_sum(n, 0), column_sum(m, 0);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                int64& value = values[i * m + j];
                cin >> value;
                row_sum[i] += value;
                column_sum[j] += value;
            }
        }

        if (n == 1 || m == 1) {
            cout << solve_line(values, k) << '\n';
            continue;
        }

        vector<int64> deficits(n * m);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                deficits[i * m + j] = row_sum[i] + column_sum[j]
                                      - 3 * values[i * m + j];
            }
        }
        nth_element(deficits.begin(), deficits.begin() + k - 1,
                    deficits.end());
        cout << ceil_nonnegative(deficits[k - 1], n + m - 3) << '\n';
    }
}
