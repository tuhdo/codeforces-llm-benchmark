#include <bits/stdc++.h>
using namespace std;

// A range [l,r] has DivMEX x exactly when every prime power below x
// occurs as a divisor of some element in the range, while at least one
// maximal prime power of x does not.
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    if (!(cin >> T)) return 0;
    while (T--) {
        int n;
        cin >> n;
        vector<int> a(n + 1);
        for (int i = 1; i <= n; ++i) cin >> a[i];

        // last[q][r] is represented by the latest position <= r whose value
        // is divisible by the prime power q.  For each r, scan x in order;
        // the prefix minimum is the largest valid left endpoint for covering
        // all prime powers smaller than x.
        vector<int> spf(n + 2);
        for (int i = 2; i <= n + 1; ++i) if (!spf[i]) {
            for (int j = i; j <= n + 1; j += i) if (!spf[j]) spf[j] = i;
        }
        vector<int> last(n + 2, 0);
        vector<char> good(n + 2, false);
        for (int r = 1; r <= n; ++r) {
            int v = a[r];
            while (v > 1) {
                int p = spf[v], q = 1;
                while (v % p == 0) { v /= p; q *= p; last[q] = r; }
            }
            int mn = n + 1;
            for (int x = 2; x <= n + 1; ++x) {
                // Incorporate all prime powers strictly below x.
                // Since x increases by one, at most one new power is added.
                int q = x - 1;
                if (q >= 2 && spf[q] == q) mn = min(mn, last[q]);
                else if (q >= 2) {
                    int p = spf[q];
                    int z = q;
                    while (z % p == 0) z /= p;
                    if (z == 1) mn = min(mn, last[q]);
                }
                if (mn == 0) continue;

                // A maximal prime power dividing x can be absent from [l,r]
                // while l remains small enough to cover every smaller power.
                int z = x;
                bool absent = false;
                while (z > 1) {
                    int p = spf[z];
                    int q = 1;
                    while (z % p == 0) { z /= p; q *= p; }
                    if (last[q] < mn) absent = true;
                }
                if (absent) good[x] = true;
            }
        }
        vector<int> ans;
        for (int x = 2; x <= n + 1; ++x) if (good[x]) ans.push_back(x);
        cout << ans.size() << '\n';
        for (int x : ans) cout << x << ' ';
        cout << '\n';
    }
}
