#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    constexpr int MAX_N = 200000;
    vector<int> spf(MAX_N + 1);
    for (int p = 2; p <= MAX_N; ++p) {
        if (spf[p] != 0) continue;
        for (int x = p; x <= MAX_N; x += p) {
            if (spf[x] == 0) spf[x] = p;
        }
    }

    int t;
    cin >> t;
    while (t--) {
        int n, k;
        cin >> n >> k;

        vector<int> dp(n + 1, 0);
        for (int x = k + 1; x <= n; ++x) {
            dp[x] = x - 1;
            int remaining = x;
            while (remaining > 1) {
                int p = spf[remaining];
                dp[x] = min(dp[x], 1 + p * dp[x / p]);
                do {
                    remaining /= p;
                } while (remaining % p == 0);
            }
        }

        long long answer = 0;
        for (int i = 0; i < n; ++i) {
            int x;
            cin >> x;
            answer += dp[x];
        }
        cout << answer << '\n';
    }
}
