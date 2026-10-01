#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int64 mod_pow(int64 base, int64 exponent, int64 mod) {
    int64 result = 1;
    while (exponent > 0) {
        if (exponent & 1) result = result * base % mod;
        base = base * base % mod;
        exponent >>= 1;
    }
    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        int64 mod;
        cin >> n >> mod;

        vector<int64> fact(2 * n + 2), inv_fact(2 * n + 2);
        fact[0] = 1;
        for (int i = 1; i <= 2 * n + 1; ++i)
            fact[i] = fact[i - 1] * i % mod;
        inv_fact[2 * n + 1] = mod_pow(fact[2 * n + 1], mod - 2, mod);
        for (int i = 2 * n + 1; i > 0; --i)
            inv_fact[i - 1] = inv_fact[i] * i % mod;

        auto choose = [&](int a, int b) -> int64 {
            if (b < 0 || b > a) return 0;
            return fact[a] * inv_fact[b] % mod * inv_fact[a - b] % mod;
        };

        vector<int64> a(n + 1);
        for (int i = 0; i <= n; ++i) {
            int64 catalan = (choose(2 * i, i) - choose(2 * i, i + 1) + mod) % mod;
            a[i] = catalan * catalan % mod;
        }

        vector<int64> moments(n + 1);
        vector<int64> power(1, 1);
        for (int r = 0; r <= n; ++r) {
            int degree = n - r;
            vector<int64> next(degree + 1);
            for (int i = 0; i <= degree; ++i) {
                for (int j = 0; j <= i && j < (int)power.size(); ++j) {
                    next[i] = (next[i] + power[j] * a[i - j]) % mod;
                }
            }
            power.swap(next);
            int64 inverse = fact[r] * inv_fact[r + 1] % mod;
            moments[r] = choose(2 * n, r) * inverse % mod * power[degree] % mod;
        }

        for (int k = 0; k <= n; ++k) {
            int64 answer = 0;
            for (int r = k; r <= n; ++r) {
                int64 term = choose(r, k) * moments[r] % mod;
                if ((r - k) & 1) answer = (answer - term + mod) % mod;
                else answer = (answer + term) % mod;
            }
            cout << answer << (k == n ? '\n' : ' ');
        }
    }
}
