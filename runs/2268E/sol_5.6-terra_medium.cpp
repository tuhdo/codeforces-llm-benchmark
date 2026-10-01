#include <bits/stdc++.h>
using namespace std;

static constexpr int MOD = 998244353;
static constexpr int ROOT = 3;

int mod_pow(int a, int e) {
    int r = 1;
    while (e) {
        if (e & 1) r = (long long)r * a % MOD;
        a = (long long)a * a % MOD;
        e >>= 1;
    }
    return r;
}

void ntt(vector<int>& a, bool invert) {
    const int n = (int)a.size();
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }
    for (int len = 2; len <= n; len <<= 1) {
        int wlen = mod_pow(ROOT, (MOD - 1) / len);
        if (invert) wlen = mod_pow(wlen, MOD - 2);
        for (int i = 0; i < n; i += len) {
            long long w = 1;
            int half = len >> 1;
            for (int j = 0; j < half; ++j) {
                int u = a[i + j];
                int v = w * a[i + j + half] % MOD;
                a[i + j] = u + v < MOD ? u + v : u + v - MOD;
                a[i + j + half] = u - v >= 0 ? u - v : u - v + MOD;
                w = w * wlen % MOD;
            }
        }
    }
    if (invert) {
        int inv_n = mod_pow(n, MOD - 2);
        for (int& x : a) x = (long long)x * inv_n % MOD;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int MAXN = 200000;
    vector<int> fact(2 * MAXN + 1, 1), ifact(2 * MAXN + 1, 1), inv(MAXN + 2, 1);
    for (int i = 1; i <= 2 * MAXN; ++i) fact[i] = (long long)fact[i - 1] * i % MOD;
    ifact[2 * MAXN] = mod_pow(fact[2 * MAXN], MOD - 2);
    for (int i = 2 * MAXN; i; --i) ifact[i - 1] = (long long)ifact[i] * i % MOD;
    for (int i = 2; i <= MAXN + 1; ++i) inv[i] = MOD - (long long)(MOD / i) * inv[MOD % i] % MOD;

    vector<int> cat(MAXN + 1);
    for (int i = 0; i <= MAXN; ++i) {
        cat[i] = (long long)fact[2 * i] * ifact[i] % MOD * ifact[i] % MOD * inv[i + 1] % MOD;
    }

    int tc;
    cin >> tc;
    while (tc--) {
        int n;
        cin >> n;
        vector<int> a(n);
        int total_xor = 0;
        for (int& x : a) {
            cin >> x;
            total_xor ^= x;
        }

        vector<int> weight(n);
        for (int m = 1; m < n; ++m) weight[m] = (long long)cat[m] * cat[n - m] % MOD;

        int size = 1;
        while (size < 2 * n + 1) size <<= 1;
        long long ans = 0;
        const int all_edges = (long long)(n - 1) * cat[n] % MOD;

        for (int bit = 0; bit < 18; ++bit) {
            if ((total_xor >> bit) & 1) {
                ans += (long long)(1 << bit) * all_edges % MOD;
                continue;
            }

            // s[i] is +1/-1 according to the bit of prefix_xor[0..i].
            // Its autocorrelation at distance m tells how many intervals of
            // length m have this XOR bit set.
            vector<int> transformed(size, 0);
            int pref = 0;
            transformed[0] = 1;
            for (int i = 1; i <= n; ++i) {
                pref ^= a[i - 1];
                transformed[i] = ((pref >> bit) & 1) ? MOD - 1 : 1;
            }
            ntt(transformed, false);

            vector<int> product(size);
            int phase = mod_pow(ROOT, (n) * ((MOD - 1) / size) % (MOD - 1));
            long long phase_power = 1;
            for (int k = 0; k < size; ++k) {
                int mirrored = (k == 0 ? 0 : size - k);
                product[k] = phase_power * transformed[k] % MOD * transformed[mirrored] % MOD;
                phase_power = phase_power * phase % MOD;
            }
            ntt(product, true);

            long long weighted_intervals = 0;
            for (int m = 1; m < n; ++m) {
                int correlation = product[n + m];
                if (correlation > MOD / 2) correlation -= MOD;
                // Among n-m+1 pairs, (count - correlation) / 2 differ.
                int different = ((n - m + 1 - correlation) % MOD + MOD) % MOD;
                different = (long long)different * ((MOD + 1) / 2) % MOD;
                weighted_intervals += (long long)weight[m] * different % MOD;
                if (weighted_intervals >= MOD) weighted_intervals -= MOD;
            }
            ans += (long long)(1 << bit) * 2 % MOD * weighted_intervals % MOD;
        }
        cout << ans % MOD << '\n';
    }
}
