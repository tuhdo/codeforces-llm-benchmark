#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

static constexpr int MOD = 1'000'000'007;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int MAXN = 200'000;
    vector<int64> fact(MAXN + 1, 1);
    for (int i = 1; i <= MAXN; ++i) {
        fact[i] = fact[i - 1] * i % MOD;
    }

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        vector<int64> a(n);
        for (auto &x : a) cin >> x;

        int k = n / 2;
        int other = k - (n % 2 == 0 ? 2 : 1);
        int64 ways = fact[k] * fact[other] % MOD;

        set<int64> candidates = {
            a[n - 1] - a[n - 2],
            a[n - 1] - a[n - 3]
        };

        int64 answer = 0;
        for (int64 x : candidates) {
            vector<int64> constructed;
            constructed.reserve(n);

            for (int i = 0; i < k; ++i) {
                constructed.push_back(static_cast<int64>(i) * x);
            }
            for (int i = 0; i < n - k; ++i) {
                constructed.push_back(a.back() - static_cast<int64>(i) * x);
            }

            sort(constructed.begin(), constructed.end());
            if (constructed == a) {
                answer += ways;
                if (answer >= MOD) answer -= MOD;
            }
        }

        cout << answer << '\n';
    }
    return 0;
}
