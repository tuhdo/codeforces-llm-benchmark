#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int64 ceilPositive(int64 value, int64 divisor) {
    if (value <= 0) return 0;
    return (value + divisor - 1) / divisor;
}

int64 favorEndpoint(const vector<int64>& sortedDeficits, int64 endpoint,
                   int k, int64 reduction) {
    if (k == 1) return ceilPositive(endpoint, reduction + 1);

    // The (k - 1)-th smallest deficit after removing this endpoint.
    int64 other = sortedDeficits[k - 2];
    if (endpoint <= other) other = sortedDeficits[k - 1];

    return max({ceilPositive(other, reduction),
                ceilPositive(endpoint, reduction + 1),
                ceilPositive(endpoint + other, 2 * reduction)});
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n, m, k;
        cin >> n >> m >> k;
        int size = n * m;

        vector<int64> values(size), rows(n, 0), columns(m, 0);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                cin >> values[i * m + j];
                rows[i] += values[i * m + j];
                columns[j] += values[i * m + j];
            }
        }

        if (size == 1) {
            cout << (values[0] >= 0 ? 0 : -1) << '\n';
            continue;
        }
        if (size == 2) {
            cout << (k == 1 ? 0LL : abs(values[0] - values[1])) << '\n';
            continue;
        }

        vector<int64> deficits(size);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                int index = i * m + j;
                deficits[index] = rows[i] + columns[j] - 3 * values[index];
            }
        }
        int64 first = deficits.front(), last = deficits.back();
        sort(deficits.begin(), deficits.end());

        int64 reduction = n + m - 3;
        int64 answer = ceilPositive(deficits[k - 1], reduction);
        if (n == 1 || m == 1) {
            answer = min(answer, favorEndpoint(deficits, first, k, reduction));
            answer = min(answer, favorEndpoint(deficits, last, k, reduction));
        }
        cout << answer << '\n';
    }
    return 0;
}
