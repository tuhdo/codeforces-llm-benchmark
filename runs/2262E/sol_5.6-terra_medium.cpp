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
        int64 mod;
        cin >> n >> mod;

        vector<int64> fact(2 * n + 1, 1), invFact(2 * n + 1, 1);
        for (int i = 1; i <= 2 * n; ++i) fact[i] = fact[i - 1] * i % mod;
        auto power = [&](int64 a, int64 e) {
            int64 result = 1;
            while (e) {
                if (e & 1) result = result * a % mod;
                a = a * a % mod;
                e >>= 1;
            }
            return result;
        };
        invFact[2 * n] = power(fact[2 * n], mod - 2);
        for (int i = 2 * n; i > 0; --i) invFact[i - 1] = invFact[i] * i % mod;

        auto choose = [&](int a, int b) -> int64 {
            if (b < 0 || b > a) return 0;
            return fact[a] * invFact[b] % mod * invFact[a - b] % mod;
        };

        // D[x] = sum Catalan(i)^2 x^i.
        vector<int64> d(n + 1);
        for (int i = 0; i <= n; ++i) {
            int64 catalan = choose(2 * i, i) * power(i + 1, mod - 2) % mod;
            d[i] = catalan * catalan % mod;
        }

        // marked[r] counts pairs together with a choice of r common pairs.
        // For m = n-r, it is C(2n,r)/(r+1) * [x^m] D(x)^(r+1).
        vector<int64> marked(n + 1);
        vector<int64> previous(n + 1), current(n + 1);
        previous[0] = 1;
        for (int exponent = 1; exponent <= n + 1; ++exponent) {
            int maxDegree = n - exponent + 1;
            fill(current.begin(), current.end(), 0);
            for (int degree = 0; degree <= maxDegree; ++degree) {
                int64 value = 0;
                for (int part = 0; part <= degree; ++part) {
                    value = (value + d[part] * previous[degree - part]) % mod;
                }
                current[degree] = value;
            }
            int r = exponent - 1;
            int64 factor = choose(2 * n, r) * power(exponent, mod - 2) % mod;
            marked[r] = factor * current[maxDegree] % mod;
            swap(previous, current);
        }

        // marked[r] = sum_{k >= r} C(k,r) * exact[k].
        vector<int64> exact(n + 1);
        for (int k = 0; k <= n; ++k) {
            int64 value = 0;
            for (int r = k; r <= n; ++r) {
                int64 term = choose(r, k) * marked[r] % mod;
                if ((r - k) & 1) value -= term;
                else value += term;
                value %= mod;
            }
            if (value < 0) value += mod;
            exact[k] = value;
        }

        for (int k = 0; k <= n; ++k) cout << exact[k] << ' ';
        cout << '\n';
    }
}
