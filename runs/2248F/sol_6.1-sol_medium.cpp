#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int64 required(int64 value, int64 rate) {
    return value <= 0 ? 0 : (value + rate - 1) / rate;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n, m, k;
        cin >> n >> m >> k;
        int size = n * m;
        vector<int64> v(size), row(n), col(m);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                cin >> v[i * m + j];
                row[i] += v[i * m + j];
                col[j] += v[i * m + j];
            }
        }

        if (n >= 2 && m >= 2) {
            vector<int64> deficit(size);
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < m; ++j)
                    deficit[i * m + j] = row[i] + col[j] - 3 * v[i * m + j];
            nth_element(deficit.begin(), deficit.begin() + k - 1, deficit.end());
            cout << required(deficit[k - 1], n + m - 3) << '\n';
            continue;
        }

        if (size == 1) {
            cout << (v[0] >= 0 ? 0 : -1) << '\n';
            continue;
        }
        if (size == 2) {
            cout << (k == 1 ? 0 : abs(v[0] - v[1])) << '\n';
            continue;
        }

        int64 sum = accumulate(v.begin(), v.end(), 0LL);
        int64 left = sum - 2 * v.front();
        int64 right = sum - 2 * v.back();
        vector<int64> inner;
        for (int i = 1; i + 1 < size; ++i)
            inner.push_back(sum - 2 * v[i]);
        sort(inner.begin(), inner.end());
        int64 d = size - 2;
        int64 answer = LLONG_MAX;

        // Choose which endpoints must become peaks. The remaining peaks
        // come from the internal cells with the smallest deficits.
        for (int mask = 0; mask < 4; ++mask) {
            int endpoints = __builtin_popcount(static_cast<unsigned>(mask));
            if (endpoints > k) continue;
            int count = k - endpoints;
            if (count > static_cast<int>(inner.size())) continue;

            int64 operations = 0;
            vector<int64> chosen;
            if (mask & 1) chosen.push_back(left);
            if (mask & 2) chosen.push_back(right);
            for (int64 a : chosen)
                operations = max(operations, required(a, d + 1));
            if (chosen.size() == 2)
                operations = max(operations, required(left + right, 2 * d));
            if (count > 0) {
                int64 b = inner[count - 1];
                operations = max(operations, required(b, d));
                for (int64 a : chosen)
                    operations = max(operations, required(a + b, 2 * d));
            }
            answer = min(answer, operations);
        }
        cout << answer << '\n';
    }
}
