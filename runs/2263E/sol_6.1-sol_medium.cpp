#include <bits/stdc++.h>
using namespace std;

constexpr long long MOD = 1000000007;

bool valid_difference(const vector<long long>& a, long long d) {
    int n = static_cast<int>(a.size());
    int low_count = (n - 2) / 2;
    int high_count = (n - 1) / 2;
    long long maximum = a.back();
    if (d <= 0 || high_count * d >= maximum) return false;

    // Merge d, 2d, ... and maximum-high_count*d, ..., maximum-d.
    int low = 1, high = high_count;
    for (int i = 1; i < n - 1; ++i) {
        long long x = low <= low_count ? low * d : LLONG_MAX;
        long long y = high >= 1 ? maximum - high * d : LLONG_MAX;
        if (x == y || min(x, y) != a[i]) return false;
        if (x < y) ++low;
        else --high;
    }
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<long long> factorial(200001, 1);
    for (int i = 1; i <= 200000; ++i) {
        factorial[i] = factorial[i - 1] * i % MOD;
    }

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<long long> a(n);
        for (auto& x : a) cin >> x;

        long long first = a[1];
        long long maximum = a.back();
        int high_count = (n - 1) / 2;
        int candidates = valid_difference(a, first);
        if ((maximum - first) % high_count == 0) {
            long long d = (maximum - first) / high_count;
            if (d != first) candidates += valid_difference(a, d);
        }

        long long answer = factorial[n / 2] * factorial[(n - 3) / 2] % MOD;
        cout << answer * candidates % MOD << '\n';
    }
}
