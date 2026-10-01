#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int MAXN = 200000 + 10;
    vector<int> spf(MAXN);
    for (int i = 2; i < MAXN; ++i) if (!spf[i]) {
        spf[i] = i;
        if (1LL * i * i < MAXN)
            for (int j = i * i; j < MAXN; j += i)
                if (!spf[j]) spf[j] = i;
    }

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        vector<int> a(n);
        for (int &v : a) cin >> v;

        // Prime powers are exactly the possible values of f.
        vector<int> candidates;
        vector<int> primePowers;
        for (int p = 2; p <= n + 1; ++p) {
            if (spf[p] == p) {
                long long q = p;
                while (q <= n + 1) {
                    primePowers.push_back((int)q);
                    q *= p;
                }
            }
        }
        sort(primePowers.begin(), primePowers.end());
        int firstAbove = n + 1;
        for (int x = 2; ; ++x) {
            bool pp = false;
            if (x < MAXN && spf[x] == x) pp = true;
            else if (x < MAXN) {
                int y = x, p = spf[x];
                while (y % p == 0) y /= p;
                pp = (y == 1);
            }
            if (pp && x > n) { firstAbove = x; break; }
        }
        for (int x : primePowers) if (x <= firstAbove) candidates.push_back(x);
        if (find(candidates.begin(), candidates.end(), firstAbove) == candidates.end())
            candidates.push_back(firstAbove);
        sort(candidates.begin(), candidates.end());

        vector<vector<int>> factors(n + 1);
        for (int v = 1; v <= n; ++v) {
            int z = v;
            while (z > 1) {
                int p = spf[z], q = 1;
                while (z % p == 0) { z /= p; q *= p; }
                for (int u = p; u <= q; u *= p) factors[v].push_back(u);
            }
        }

        vector<int> mark(n + 2, 0);
        int stamp = 0;
        vector<int> answer;
        for (int x : candidates) {
            int need = lower_bound(primePowers.begin(), primePowers.end(), x) - primePowers.begin();
            ++stamp;
            int covered = 0;
            bool ok = false;
            for (int i = 0; i <= n; ++i) {
                if (i == n || a[i] % x == 0) {
                    if (covered == need) ok = true;
                    covered = 0;
                    ++stamp;
                    if (i == n) break;
                } else {
                    for (int q : factors[a[i]]) if (q < x && mark[q] != stamp) {
                        mark[q] = stamp;
                        ++covered;
                    }
                }
            }
            if (ok) answer.push_back(x);
        }

        cout << answer.size() << '\n';
        for (int i = 0; i < (int)answer.size(); ++i)
            cout << answer[i] << (i + 1 == (int)answer.size() ? '\n' : ' ');
    }
}
