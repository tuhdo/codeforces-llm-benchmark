#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

static int64 ceil_div_nonnegative(int64 x, int64 d) {
    return x <= 0 ? 0 : (x + d - 1) / d;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n, m, k;
        cin >> n >> m >> k;
        vector<vector<int64>> v(n, vector<int64>(m));
        vector<int64> row(n), col(m);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                cin >> v[i][j];
                row[i] += v[i][j];
                col[j] += v[i][j];
            }
        }

        // A one-cell matrix cannot improve: its peak condition is v[0][0] >= 0.
        if (n == 1 && m == 1) {
            cout << (v[0][0] >= 0 ? 0 : -1) << '\n';
            continue;
        }

        // With at least two rows and two columns, a full-matrix operation gives
        // every cell the largest possible increase in its peak slack.
        if (n > 1 && m > 1) {
            const int64 gain = n + m - 3;
            vector<int64> need;
            need.reserve(n * m);
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    int64 slack = 3 * v[i][j] - row[i] - col[j];
                    need.push_back(ceil_div_nonnegative(-slack, gain));
                }
            }
            nth_element(need.begin(), need.begin() + (k - 1), need.end());
            cout << need[k - 1] << '\n';
            continue;
        }

        // One-dimensional case.  Let g[i] = 2*a[i] - sum(a).  An interval of
        // length at most len-2 is dominated by decrementing the whole array.
        // Thus only the whole array and the two intervals omitting an endpoint
        // need be considered.
        const int len = max(n, m);
        vector<int64> a(len);
        for (int i = 0; i < len; ++i) a[i] = n == 1 ? v[0][i] : v[i][0];
        int64 sum = accumulate(a.begin(), a.end(), int64(0));
        vector<int64> g(len);
        for (int i = 0; i < len; ++i) g[i] = 2 * a[i] - sum;

        const int64 whole_gain = len - 2;

        auto possible = [&](int64 q) {
            // d is (# intervals omitting the first endpoint) minus
            // (# intervals omitting the last endpoint).  Taking |d| such
            // intervals is always best for the interior cells.
            vector<int64> candidates = {0};
            int64 left_need = -g[0] - whole_gain * q;
            int64 right_limit = g[len - 1] + whole_gain * q;
            if (left_need > 0) candidates.push_back(left_need);
            if (right_limit < 0) candidates.push_back(right_limit);
            if (left_need <= right_limit) {
                candidates.push_back(max(left_need, min<int64>(0, right_limit)));
            }

            for (int64 d : candidates) {
                int64 used = llabs(d);
                if (used > q) continue;
                int count = 0;
                if (g[0] + whole_gain * q + d >= 0) ++count;
                if (g[len - 1] + whole_gain * q - d >= 0) ++count;
                for (int i = 1; i + 1 < len; ++i) {
                    if (g[i] + whole_gain * q - used >= 0) ++count;
                }
                if (count >= k) return true;
            }
            return false;
        };

        int64 lo = 0;
        int64 hi = 1;
        while (!possible(hi)) hi *= 2;
        while (lo < hi) {
            int64 mid = lo + (hi - lo) / 2;
            if (possible(mid)) hi = mid;
            else lo = mid + 1;
        }
        cout << lo << '\n';
    }
}
