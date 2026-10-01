#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n, m, k;
        cin >> n >> m >> k;
        vector<vector<int64>> a(n, vector<int64>(m));
        vector<int64> row(n, 0), col(m, 0);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                cin >> a[i][j];
                row[i] += a[i][j];
                col[j] += a[i][j];
            }
        }

        vector<int64> score;
        score.reserve((size_t)n * m);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j)
                score.push_back(3 * a[i][j] - row[i] - col[j]);

        if (n >= 2 && m >= 2) {
            sort(score.begin(), score.end(), greater<int64>());
            int64 need = max<int64>(0, -score[k - 1]);
            int64 gain = n + m - 3;
            cout << (need + gain - 1) / gain << '\n';
            continue;
        }

        // The remaining matrix is a one-dimensional array (or a single cell).
        int len = max(n, m);
        if (len == 1) {
            cout << (score[0] >= 0 ? 0 : -1) << '\n';
            continue;
        }

        vector<int64> q(len);
        if (n == 1) {
            for (int j = 0; j < len; ++j) q[j] = score[j];
        } else {
            for (int i = 0; i < len; ++i) q[i] = score[i];
        }

        int64 first = q.front();
        int64 last = q.back();
        vector<int64> middle(q.begin() + 1, q.end() - 1);
        sort(middle.begin(), middle.end());

        auto feasible = [&](int64 ops) -> bool {
            const int64 g = len - 2;
            const int64 base = g * ops;
            // x operations on [0,len-2], y on [1,len-1].
            // With d=y-x and s=x+y, endpoint score changes are +d/-d
            // around the full-interval baseline; each such operation costs
            // one unit of baseline gain on interior cells.
            const int64 lowerD = -first - base;
            const int64 upperD = last + base;

            vector<int64> starts{0};
            starts.reserve(middle.size() + 2);
            for (int64 v : middle) {
                int64 b = v + base + 1;
                if (b > 0 && b <= ops) starts.push_back(b);
            }
            sort(starts.begin(), starts.end());
            starts.erase(unique(starts.begin(), starts.end()), starts.end());
            starts.push_back(ops + 1);

            auto endpoints_possible = [&](int64 s, int need) -> bool {
                if (need <= 0) return true;
                if (need == 1) {
                    return s >= min(lowerD, -upperD);
                }
                if (need != 2 || lowerD > upperD) return false;
                int64 lo = max(lowerD, -s);
                int64 hi = min(upperD, s);
                return lo <= hi && ((lo & 1LL) == (s & 1LL) || lo < hi);
            };

            for (size_t z = 0; z + 1 < starts.size(); ++z) {
                int64 left = starts[z];
                int64 right = starts[z + 1] - 1;
                if (left > right) continue;

                int64 threshold = left - base;
                int64 goodMiddle = middle.end() - lower_bound(middle.begin(), middle.end(), threshold);
                int needEndpoints = k - (int)goodMiddle;
                if (needEndpoints <= 0) return true;
                if (needEndpoints > 2) continue;

                if (needEndpoints == 1) {
                    int64 needS = min(lowerD, -upperD);
                    if (max(left, needS) <= right) return true;
                } else {
                    if (lowerD > upperD) continue;
                    int64 startS = max({left, int64(0), lowerD, -upperD});
                    if (startS > right) continue;
                    if (endpoints_possible(startS, 2) ||
                        (startS + 1 <= right && endpoints_possible(startS + 1, 2)))
                        return true;
                }
            }
            return false;
        };

        int64 hi;
        if (len >= 3) {
            int64 minQ = *min_element(q.begin(), q.end());
            hi = (max<int64>(0, -minQ) + (len - 3)) / (len - 2);
        } else {
            hi = max<int64>(1, max(llabs(first), llabs(last)));
        }
        int64 lo = 0;
        while (lo < hi) {
            int64 mid = lo + (hi - lo) / 2;
            if (feasible(mid)) hi = mid;
            else lo = mid + 1;
        }
        cout << (feasible(lo) ? lo : -1) << '\n';
    }
    return 0;
}
