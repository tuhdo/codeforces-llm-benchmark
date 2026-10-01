#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int64 n, d;
        int m;
        cin >> n >> m >> d;

        vector<int64> p(m), pref(m + 1, 0);
        for (int i = 0; i < m; ++i) {
            int64 r;
            cin >> p[i] >> r;
            pref[i + 1] = pref[i] + r;
        }

        const int64 wholeBlock = pref[m];

        auto bonusInPrefix = [&](int64 length) {
            int count = upper_bound(p.begin(), p.end(), length) - p.begin();
            return pref[count];
        };

        bool possible = false;

        // A useful run may end at a bonus position before n.  A run of n
        // ones is identical to an untouched complete block, so it can be
        // removed from a witness.
        for (int i = 0; i < m && !possible; ++i) {
            if (p[i] == n) break;
            for (int j = 0; j < m; ++j) {
                if (p[j] == n) break;

                // The candidate is 1^p[i] 0 1^p[j].
                int64 length = p[i] + 1 + p[j];
                int64 allOnes = (length / n) * wholeBlock
                              + bonusInPrefix(length % n);
                int64 candidate = pref[i + 1] + pref[j + 1] - d;

                if (candidate > allOnes) {
                    possible = true;
                    break;
                }
            }
        }

        cout << (possible ? "YES\n" : "NO\n");
    }
}
