#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

static int64 mod_pow(int64 a, int64 e, int64 mod) {
    int64 result = 1;
    while (e > 0) {
        if (e & 1) result = result * a % mod;
        a = a * a % mod;
        e >>= 1;
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

        vector<int64> fact(2 * n + 1), inv_fact(2 * n + 1);
        fact[0] = 1;
        for (int i = 1; i <= 2 * n; ++i) fact[i] = fact[i - 1] * i % mod;
        inv_fact[2 * n] = mod_pow(fact[2 * n], mod - 2, mod);
        for (int i = 2 * n; i > 0; --i) inv_fact[i - 1] = inv_fact[i] * i % mod;

        vector<int64> catalan_sq(n + 1);
        for (int r = 0; r <= n; ++r) {
            int64 catalan = fact[2 * r] * inv_fact[r] % mod * inv_fact[r] % mod;
            catalan = catalan * mod_pow(r + 1, mod - 2, mod) % mod;
            catalan_sq[r] = catalan * catalan % mod;
        }

        // B[q] = sum over pairs (s,t) of binom(common(s,t), q).
        // If C(x) = sum Catalan[r]^2 x^r, then
        // B[q] = binom(2n,q)/(q+1) * [x^(n-q)] C(x)^(q+1).
        vector<int64> moments(n + 1);
        vector<int64> power = catalan_sq; // Coefficients of C(x)^(q+1), truncated as needed.
        for (int q = 0; q <= n; ++q) {
            int degree = n - q;
            int64 choose = fact[2 * n] * inv_fact[q] % mod * inv_fact[2 * n - q] % mod;
            moments[q] = choose * mod_pow(q + 1, mod - 2, mod) % mod * power[degree] % mod;

            if (q == n) break;
            int next_degree = degree - 1;
            vector<int64> next_power(next_degree + 1, 0);
            for (int i = 0; i <= next_degree; ++i) {
                int64 value = 0;
                for (int j = 0; j <= i; ++j) {
                    value = (value + power[j] * catalan_sq[i - j]) % mod;
                }
                next_power[i] = value;
            }
            power.swap(next_power);
        }

        // Binomial inversion: exact[k] = sum_{q>=k} (-1)^(q-k) C(q,k) B[q].
        vector<int64> exact(n + 1, 0);
        for (int k = 0; k <= n; ++k) {
            int64 value = 0;
            for (int q = k; q <= n; ++q) {
                int64 choose = fact[q] * inv_fact[k] % mod * inv_fact[q - k] % mod;
                int64 term = choose * moments[q] % mod;
                if ((q - k) & 1) value = (value - term + mod) % mod;
                else value = (value + term) % mod;
            }
            exact[k] = value;
        }

        for (int k = 0; k <= n; ++k) {
            if (k) cout << ' ';
            cout << exact[k];
        }
        cout << '\n';
    }
    return 0;
}
