#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n, m, k;
        cin >> n >> m >> k;

        vector<vector<long long>> v(n, vector<long long>(m));
        vector<long long> row(n), col(m);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                cin >> v[i][j];
                row[i] += v[i][j];
                col[j] += v[i][j];
            }
        }

        if (n == 1 && m == 1) {
            cout << 0 << '\n';
            continue;
        }

        // With two cells, both are peaks exactly when their values are equal.
        if (n + m == 3) {
            if (k == 1) {
                cout << 0 << '\n';
            } else {
                cout << llabs(v[0][0] - v[n - 1][m - 1]) << '\n';
            }
            continue;
        }

        if (n == 1 || m == 1) {
            int len = max(n, m);
            vector<long long> a(len);
            long long sum = 0;
            for (int i = 0; i < len; ++i) {
                a[i] = n == 1 ? v[0][i] : v[i][0];
                sum += a[i];
            }
            for (long long &x : a) x = 2 * x - sum;

            const long long fullGain = len - 2;
            auto possible = [&](long long operations) {
                __int128 base = (__int128)operations * fullGain;
                int count = 0;
                for (long long x : a) {
                    if ((__int128)x + base >= 0) ++count;
                }
                if (count >= k) return true;

                // Besides full intervals, only an interval omitting one endpoint
                // can help: it gives that endpoint one extra margin unit and
                // gives every other position one less.
                for (int endpoint : {0, len - 1}) {
                    __int128 required = -(__int128)a[endpoint] - base;
                    long long extra = required <= 0 ? 0 : (required > operations ? operations + 1 : (long long)required);
                    if (extra > operations) continue;

                    int current = 1;
                    for (int i = 0; i < len; ++i) {
                        if (i != endpoint && (__int128)a[i] + base - extra >= 0) ++current;
                    }
                    if (current >= k) return true;
                }
                return false;
            };

            long long low = 0, high = 400000000000000LL;
            while (low < high) {
                long long mid = low + (high - low) / 2;
                if (possible(mid)) high = mid;
                else low = mid + 1;
            }
            cout << low << '\n';
            continue;
        }

        vector<long long> margin;
        margin.reserve(1LL * n * m);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                margin.push_back(3 * v[i][j] - row[i] - col[j]);
            }
        }
        nth_element(margin.begin(), margin.begin() + (k - 1), margin.end(), greater<>());
        long long need = max(0LL, -margin[k - 1]);
        long long gain = n + m - 3;
        cout << (need + gain - 1) / gain << '\n';
    }
}
