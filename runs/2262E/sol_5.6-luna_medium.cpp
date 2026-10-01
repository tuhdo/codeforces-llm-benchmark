#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        int mod;
        cin >> n >> mod;

        vector<int> fact(2 * n + 1), inv_fact(2 * n + 1);
        fact[0] = 1;
        for (int i = 1; i <= 2 * n; ++i) {
            fact[i] = (int)((int64)fact[i - 1] * i % mod);
        }

        auto power = [&](int base, int exponent) {
            int result = 1;
            while (exponent > 0) {
                if (exponent & 1) result = (int)((int64)result * base % mod);
                base = (int)((int64)base * base % mod);
                exponent >>= 1;
            }
            return result;
        };

        inv_fact[2 * n] = power(fact[2 * n], mod - 2);
        for (int i = 2 * n; i >= 1; --i) {
            inv_fact[i - 1] = (int)((int64)inv_fact[i] * i % mod);
        }

        auto choose = [&](int top, int bottom) {
            return (int)((int64)fact[top] * inv_fact[bottom] % mod * inv_fact[top - bottom] % mod);
        };

        // F(a, b) = a! / ((b + 1)! (a - b)!).
        auto ballot = [&](int a, int b) {
            return (int)((int64)fact[a] * inv_fact[b + 1] % mod * inv_fact[a - b] % mod);
        };

        // p[r][x] is the coefficient of z^x in
        // (sum Catalan_x^2 z^x)^(r+1).
        vector<vector<int>> p(n + 1, vector<int>(n + 1, 0));
        for (int x = 0; x <= n; ++x) {
            int cat = ballot(2 * x, x);
            p[0][x] = (int)((int64)cat * cat % mod);
        }
        for (int r = 1; r <= n; ++r) {
            for (int x = 0; x <= n; ++x) {
                int value = 0;
                for (int y = 0; y <= x; ++y) {
                    value = (value + (int)((int64)p[r - 1][x - y] * p[0][y] % mod)) % mod;
                }
                p[r][x] = value;
            }
        }

        // marked[r] counts ordered pairs with a prescribed set of r common pairings.
        vector<int> marked(n + 1);
        for (int r = 0; r <= n; ++r) {
            marked[r] = (int)((int64)ballot(2 * n, r) * p[r][n - r] % mod);
        }

        // Binomial inversion: marked[r] = sum_k C(k,r) * answer[k].
        vector<int> answer = marked;
        for (int r = n - 1; r >= 0; --r) {
            for (int k = r + 1; k <= n; ++k) {
                answer[r] -= (int)((int64)answer[k] * choose(k, r) % mod);
                if (answer[r] < 0) answer[r] += mod;
            }
        }

        for (int i = 0; i <= n; ++i) {
            if (i) cout << ' ';
            cout << answer[i];
        }
        cout << '\n';
    }
    return 0;
}
