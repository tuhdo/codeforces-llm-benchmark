#include <bits/stdc++.h>
using namespace std;

static constexpr long long MOD = 1'000'000'007LL;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<long long> a(n);
        for (auto &x : a) cin >> x;

        const int k = n / 2;
        long long ways = 1;
        for (int i = 1; i <= k; ++i) ways = ways * i % MOD;
        int second = k - (n % 2 == 0 ? 2 : 1);
        for (int i = 1; i <= second; ++i) ways = ways * i % MOD;

        set<long long> candidates = {
            a[n - 1] - a[n - 2],
            a[n - 1] - a[n - 3]
        };

        long long answer = 0;
        for (long long x : candidates) {
            vector<long long> values;
            values.reserve(n);
            for (int i = 0; i < k; ++i) values.push_back(i * x);
            for (int i = 0; i < n - k; ++i) values.push_back(a[n - 1] - i * x);
            sort(values.begin(), values.end());
            if (values == a) answer = (answer + ways) % MOD;
        }
        cout << answer << '\n';
    }
    return 0;
}
