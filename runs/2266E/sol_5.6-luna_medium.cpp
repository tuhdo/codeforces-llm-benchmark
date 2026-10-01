#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;

    vector<pair<int, vector<int>>> tests;
    tests.reserve(t);
    int global_max = 1;

    for (int tc = 0; tc < t; ++tc) {
        int n, k;
        cin >> n >> k;
        vector<int> a(n);
        for (int &x : a) {
            cin >> x;
            global_max = max(global_max, x);
        }
        tests.push_back({k, move(a)});
    }

    vector<int> spf(global_max + 1);
    for (int i = 2; i <= global_max; ++i) {
        if (spf[i] == 0) {
            spf[i] = i;
            if (1LL * i * i <= global_max) {
                for (long long j = 1LL * i * i; j <= global_max; j += i) {
                    if (spf[j] == 0) spf[j] = i;
                }
            }
        }
    }

    for (const auto &[k, a] : tests) {
        int n = static_cast<int>(a.size());
        vector<long long> dp(n + 1, 0);

        for (int x = k + 1; x <= n; ++x) {
            long long best = LLONG_MAX;
            int y = x;
            while (y > 1) {
                int p = spf[y];
                best = min(best, 1LL + 1LL * p * dp[x / p]);
                while (y % p == 0) y /= p;
            }
            dp[x] = best;
        }

        long long answer = 0;
        for (int x : a) answer += dp[x];
        cout << answer << '\n';
    }

    return 0;
}
