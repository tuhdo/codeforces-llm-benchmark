#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int64 n, d;
        int m;
        cin >> n >> m >> d;

        vector<int64> p(m), r(m);
        for (int i = 0; i < m; ++i) {
            cin >> p[i] >> r[i];
        }

        vector<int64> doubled(2 * m), pref(2 * m + 1, 0);
        for (int i = 0; i < 2 * m; ++i) {
            doubled[i] = p[i % m] + (i >= m ? n : 0);
            pref[i + 1] = pref[i] + r[i % m];
        }
        vector<int64> starts;
        for (int i = 0; i < m; ++i) {
            starts.push_back(p[i] == n ? 1 : p[i] + 1);
        }
        sort(starts.begin(), starts.end());

        bool possible = false;
        int64 earned = 0;

        for (int i = 0; i < m; ++i) {
            earned += r[i];
            if (p[i] == n) {
                continue;
            }

            // Try a zero, then p[i] ones.  The all-one array earns rewards
            // from a circular interval of p[i] + 1 positions in that time.
            const int64 length = p[i] + 1;
            int64 minimumBaselineReward = LLONG_MAX;
            int left = 0, right = 0;

            for (int64 start : starts) {
                int64 end = start + length - 1;
                while (left < 2 * m && doubled[left] < start) {
                    ++left;
                }
                right = max(right, left);
                while (right < 2 * m && doubled[right] <= end) {
                    ++right;
                }
                minimumBaselineReward = min(minimumBaselineReward,
                                           pref[right] - pref[left]);
            }

            if (earned - d > minimumBaselineReward) {
                possible = true;
                break;
            }
        }

        cout << (possible ? "YES\n" : "NO\n");
    }
}
