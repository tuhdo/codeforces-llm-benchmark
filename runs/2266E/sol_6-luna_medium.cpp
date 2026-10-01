#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n, k;
        cin >> n >> k;
        vector<int> a(n);
        for (int &x : a) cin >> x;

        vector<int> spf(n + 1);
        for (int i = 2; i <= n; ++i) {
            if (spf[i] == 0) {
                spf[i] = i;
                if (1LL * i * i <= n) {
                    for (long long j = 1LL * i * i; j <= n; j += i)
                        if (spf[j] == 0) spf[j] = i;
                }
            }
        }

        vector<long long> cost(n + 1, 0);
        for (int x = k + 1; x <= n; ++x) {
            long long best = LLONG_MAX;
            int y = x;
            while (y > 1) {
                int p = spf[y];
                best = min(best, 1LL + p * cost[x / p]);
                while (y % p == 0) y /= p;
            }
            cost[x] = best;
        }

        long long answer = 0;
        for (int x : a) answer += cost[x];
        cout << answer << '\n';
    }
    return 0;
}
