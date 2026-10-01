#include <bits/stdc++.h>
using namespace std;

static constexpr int MOD = 998244353;
static constexpr int ROOT = 3;

int mod_pow(long long a, long long e) {
    long long r = 1;
    while (e) {
        if (e & 1) r = r * a % MOD;
        a = a * a % MOD;
        e >>= 1;
    }
    return (int)r;
}

void ntt(vector<int>& a, bool invert) {
    int n = (int)a.size();
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }

    for (int len = 2; len <= n; len <<= 1) {
        int wlen = mod_pow(ROOT, (MOD - 1) / len);
        if (invert) wlen = mod_pow(wlen, MOD - 2);
        int half = len >> 1;
        for (int i = 0; i < n; i += len) {
            long long w = 1;
            for (int j = 0; j < half; ++j) {
                int u = a[i + j];
                int v = (int)(a[i + j + half] * w % MOD);
                int x = u + v;
                if (x >= MOD) x -= MOD;
                int y = u - v;
                if (y < 0) y += MOD;
                a[i + j] = x;
                a[i + j + half] = y;
                w = w * wlen % MOD;
            }
        }
    }

    if (invert) {
        int inv_n = mod_pow(n, MOD - 2);
        for (int& x : a) x = (int)((long long)x * inv_n % MOD);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    if (!(cin >> T)) return 0;
    while (T--) {
        int n;
        cin >> n;
        vector<int> a(n);
        for (int& x : a) cin >> x;

        vector<int> fact(2 * n + 1), invfact(2 * n + 1);
        fact[0] = 1;
        for (int i = 1; i <= 2 * n; ++i) fact[i] = (long long)fact[i - 1] * i % MOD;
        invfact[2 * n] = mod_pow(fact[2 * n], MOD - 2);
        for (int i = 2 * n; i > 0; --i) invfact[i - 1] = (long long)invfact[i] * i % MOD;
        vector<int> catalan(n + 1);
        for (int i = 0; i <= n; ++i) {
            catalan[i] = (long long)fact[2 * i] * invfact[i] % MOD * invfact[i] % MOD;
            catalan[i] = (long long)catalan[i] * mod_pow(i + 1, MOD - 2) % MOD;
        }

        // w[k] is the number of full trees in which a fixed length-k interval
        // occurs as one side of an edge.
        vector<int> w(n + 1, 0), pref_w(n + 1, 0);
        for (int k = 1; k < n; ++k) {
            w[k] = (long long)catalan[k] * catalan[n - k] % MOD;
        }
        for (int k = 1; k <= n; ++k) {
            pref_w[k] = pref_w[k - 1] + w[k];
            if (pref_w[k] >= MOD) pref_w[k] -= MOD;
        }

        int total_xor = 0;
        vector<int> px(n + 1, 0);
        for (int i = 0; i < n; ++i) {
            total_xor ^= a[i];
            px[i + 1] = px[i] ^ a[i];
        }

        long long all_interval_weight = 0;
        for (int k = 1; k < n; ++k) {
            all_interval_weight += (long long)w[k] * (n - k + 1) % MOD;
            if (all_interval_weight >= MOD) all_interval_weight -= MOD;
        }
        long long answer = all_interval_weight * total_xor % MOD;

        if (n >= 2) {
            int m = 1;
            while (m < 2 * n + 1) m <<= 1;
            vector<int> kernel(m, 0);
            for (int d = 1; d < n; ++d) {
                kernel[d] = w[d];
                kernel[m - d] = w[d];
            }
            ntt(kernel, false);
            int inv_m = mod_pow(m, MOD - 2);

            for (int bit = 0; bit < 18; ++bit) {
                if ((total_xor >> bit) & 1) continue;
                vector<int> p(m, 0);
                for (int i = 0; i <= n; ++i) p[i] = (px[i] >> bit) & 1;
                ntt(p, false);

                long long quadratic_sum = 0;
                for (int f = 0; f < m; ++f) {
                    int opposite = (m - f) & (m - 1);
                    long long term = (long long)kernel[f] * p[f] % MOD * p[opposite] % MOD;
                    quadratic_sum += term;
                    if (quadratic_sum >= MOD) quadratic_sum -= MOD;
                }
                quadratic_sum = quadratic_sum * inv_m % MOD;

                long long linear_sum = 0;
                for (int i = 0; i <= n; ++i) {
                    int coefficient = pref_w[i] + pref_w[n - i];
                    if (coefficient >= MOD) coefficient -= MOD;
                    if ((px[i] >> bit) & 1) {
                        linear_sum += coefficient;
                        if (linear_sum >= MOD) linear_sum -= MOD;
                    }
                }
                long long differing_weight = (linear_sum - quadratic_sum + MOD) % MOD;
                answer = (answer + (2LL * (1 << bit) % MOD) * differing_weight) % MOD;
            }
        }

        cout << answer % MOD << '\n';
    }
    return 0;
}

