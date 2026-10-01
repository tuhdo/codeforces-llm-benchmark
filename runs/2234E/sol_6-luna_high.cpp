#include <bits/stdc++.h>
using namespace std;

static constexpr long long MOD = 1'000'000'007LL;
static constexpr int MAX_N = 500'000;

long long mod_pow(long long base, long long exp) {
    long long result = 1;
    while (exp > 0) {
        if (exp & 1) result = result * base % MOD;
        base = base * base % MOD;
        exp >>= 1;
    }
    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<long long> fact(MAX_N + 1), inv_fact(MAX_N + 1);
    fact[0] = 1;
    for (int i = 1; i <= MAX_N; ++i) fact[i] = fact[i - 1] * i % MOD;
    inv_fact[MAX_N] = mod_pow(fact[MAX_N], MOD - 2);
    for (int i = MAX_N; i > 0; --i) inv_fact[i - 1] = inv_fact[i] * i % MOD;

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<long long> a(n);
        for (long long &x : a) cin >> x;

        long long ways = fact[n];
        bool valid = true;
        vector<pair<int, int>> intervals;
        intervals.emplace_back(0, n - 1);

        while (!intervals.empty() && valid) {
            auto [left, right] = intervals.back();
            intervals.pop_back();
            if (left > right) continue;

            const int len = right - left + 1;
            int root = -1;
            for (int d = 0; d <= (len - 1) / 2; ++d) {
                const long long required = 1LL * (d + 1) * (len - d);
                const int from_left = left + d;
                if (a[from_left] == required) {
                    root = from_left;
                    break;
                }
                const int from_right = right - d;
                if (from_right != from_left && a[from_right] == required) {
                    root = from_right;
                    break;
                }
            }

            if (root == -1) {
                valid = false;
                break;
            }

            // This interval is exactly the Cartesian subtree of its minimum.
            ways = ways * fact[len - 1] % MOD * inv_fact[len] % MOD;
            intervals.emplace_back(left, root - 1);
            intervals.emplace_back(root + 1, right);
        }

        cout << (valid ? ways : 0) << '\n';
    }
    return 0;
}
