#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
constexpr int64 MOD = 1'000'000'007;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;

    const int MAX_N = 200000;
    vector<int64> fact(MAX_N + 1, 1);
    for (int i = 1; i <= MAX_N; ++i) {
        fact[i] = fact[i - 1] * i % MOD;
    }

    while (t--) {
        int n;
        cin >> n;
        vector<int64> a(n);
        for (int64 &x : a) cin >> x;

        if (n % 2 == 1) {
            cout << 0 << '\n';
            continue;
        }

        const int k = n / 2 - 1;
        const int64 maximum = a.back();

        vector<int64> candidates = {a[1]};
        if ((maximum - a[1]) % k == 0) {
            const int64 other = (maximum - a[1]) / k;
            if (other > 0 && other != candidates[0]) candidates.push_back(other);
        }

        int good_differences = 0;
        for (int64 d : candidates) {
            if (d <= 0) continue;

            vector<int64> expected;
            expected.reserve(n);
            expected.push_back(maximum);
            for (int i = 0; i <= k; ++i) expected.push_back(d * i);
            for (int i = 1; i <= k; ++i) expected.push_back(maximum - d * i);
            sort(expected.begin(), expected.end());

            if (expected == a) ++good_differences;
        }

        const int64 arrangements = fact[k + 1] * fact[k - 1] % MOD;
        cout << arrangements * good_differences % MOD << '\n';
    }
}
