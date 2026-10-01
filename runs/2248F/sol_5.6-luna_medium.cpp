#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
static const int64 INF = (1LL << 62);

int64 ceil_div(int64 a, int64 b) {
    if (a >= 0) return (a + b - 1) / b;
    return a / b; // C++ truncation toward zero is the ceiling for a < 0.
}

// Minimum T such that x full intervals and y intervals skipping the chosen
// endpoint, with x+y=T, can satisfy both deficit bounds.
int64 one_dim_option(int64 regular, int64 special, int k, int len) {
    const int64 q = len - 2;

    // For len == 2, a full interval changes no deficit.  A skipped-endpoint
    // interval helps the special endpoint and hurts the other cells.
    if (q == 0) {
        if (k == 1) return min<int64>(0, min(regular, special) <= 0 ? 0 : INF);

        // Both cells must be peaks.  The special one needs y >= special,
        // while the other needs y <= -regular.
        int64 low = max<int64>(0, special);
        int64 high = -regular;
        if (low <= high) return low;
        return INF;
    }

    int64 lo = 0, hi = 1;
    auto feasible = [&](int64 T) {
        // q*T - y >= regular, q*T + y >= special.
        int64 low = 0;
        if (special < INF / 2)
            low = max<int64>(0, special - q * T);
        int64 high = T;
        if (regular > -INF / 2)
            high = min<int64>(high, q * T - regular);
        return low <= high;
    };

    // This branch is only called with at least one possible peak; nevertheless
    // retain a finite search bound for very large deficits.
    while (!feasible(hi)) {
        if (hi > (1LL << 60)) return INF;
        hi *= 2;
    }
    while (lo < hi) {
        int64 mid = lo + (hi - lo) / 2;
        if (feasible(mid)) hi = mid;
        else lo = mid + 1;
    }
    return lo;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n, m, k;
        cin >> n >> m >> k;
        vector<vector<int64>> a(n, vector<int64>(m));
        vector<int64> row(n), col(m);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                cin >> a[i][j];
                row[i] += a[i][j];
                col[j] += a[i][j];
            }
        }

        if (n >= 2 && m >= 2) {
            int64 step = n + m - 3;
            vector<int64> need;
            need.reserve(n * m);
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    int64 deficit = row[i] + col[j] - 3 * a[i][j];
                    need.push_back(max<int64>(0, ceil_div(deficit, step)));
                }
            }
            nth_element(need.begin(), need.begin() + (k - 1), need.end());
            cout << need[k - 1] << '\n';
            continue;
        }

        // Reduce the one-dimensional case to an array.  For n=m=1, a peak
        // means the sole value is nonnegative and operations cannot help.
        vector<int64> d;
        if (n == 1 && m == 1) {
            cout << (a[0][0] >= 0 ? 0 : -1) << '\n';
            continue;
        }
        if (n == 1) {
            int64 total = 0;
            for (auto x : a[0]) total += x;
            for (int j = 0; j < m; ++j) d.push_back(total - 2 * a[0][j]);
        } else {
            int64 total = 0;
            for (auto x : a) total += x[0];
            for (int i = 0; i < n; ++i) d.push_back(total - 2 * a[i][0]);
            m = n;
        }

        int len = (int)d.size();
        int64 answer = INF;
        if (k == 1) {
            for (auto x : d) if (x <= 0) answer = 0;
        }

        // Try each endpoint as the cell specially helped by [2..len] (or
        // [1..len-1]).  If that endpoint is not selected, k regular cells
        // are needed; otherwise k-1 regular cells plus the endpoint suffice.
        for (int endpoint : {0, len - 1}) {
            vector<int64> other;
            for (int i = 0; i < len; ++i)
                if (i != endpoint) other.push_back(d[i]);
            sort(other.begin(), other.end());

            if (k <= (int)other.size()) {
                answer = min(answer, one_dim_option(other[k - 1], INF, k, len));
            }
            if (k >= 1 && k - 1 <= (int)other.size()) {
                int64 regular = (k == 1 ? -INF : other[k - 2]);
                answer = min(answer, one_dim_option(regular, d[endpoint], k, len));
            }
        }

        cout << (answer == INF ? -1 : answer) << '\n';
    }
}
