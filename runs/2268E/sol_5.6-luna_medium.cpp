#include <bits/stdc++.h>
using namespace std;

static const int MOD = 998244353;
static const int G = 3;

int mod_pow(int a, int e) {
    long long r = 1;
    while (e > 0) {
        if (e & 1) r = r * a % MOD;
        a = (long long)a * a % MOD;
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
        int wlen = mod_pow(G, (MOD - 1) / len);
        if (invert) wlen = mod_pow(wlen, MOD - 2);
        for (int i = 0; i < n; i += len) {
            long long w = 1;
            int half = len >> 1;
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

vector<int> convolution(vector<int> a, vector<int> b) {
    int need = (int)a.size() + (int)b.size() - 1;
    int n = 1;
    while (n < need) n <<= 1;
    a.resize(n);
    b.resize(n);
    ntt(a, false);
    ntt(b, false);
    for (int i = 0; i < n; ++i) a[i] = (long long)a[i] * b[i] % MOD;
    ntt(a, true);
    a.resize(need);
    return a;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int MAXN = 200000;
    vector<int> fact(2 * MAXN + 1), inv_fact(2 * MAXN + 1);
    fact[0] = 1;
    for (int i = 1; i <= 2 * MAXN; ++i) fact[i] = (long long)fact[i - 1] * i % MOD;
    inv_fact[2 * MAXN] = mod_pow(fact[2 * MAXN], MOD - 2);
    for (int i = 2 * MAXN; i >= 1; --i) inv_fact[i - 1] = (long long)inv_fact[i] * i % MOD;

    auto catalan = [&](int n) {
        return (int)((long long)fact[2 * n] * inv_fact[n] % MOD * inv_fact[n + 1] % MOD);
    };

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<int> a(n);
        for (int& x : a) cin >> x;

        vector<int> pref(n + 1, 0);
        for (int i = 0; i < n; ++i) pref[i + 1] = pref[i] ^ a[i];
        int total_xor = pref[n];

        vector<int> weight(n + 1);
        for (int len = 1; len <= n; ++len)
            weight[len] = (long long)catalan(len) * catalan(n - len) % MOD;

        long long answer = 0;
        for (int bit = 0; bit < 18; ++bit) {
            vector<int> signed_prefix(n + 1), reversed(n + 1);
            for (int i = 0; i <= n; ++i) {
                int value = ((pref[i] >> bit) & 1) ? MOD - 1 : 1;
                signed_prefix[i] = value;
                reversed[n - i] = value;
            }
            vector<int> corr = convolution(move(signed_prefix), move(reversed));

            long long bit_sum = 0;
            bool total_bit = (total_xor >> bit) & 1;
            for (int len = 1; len < n; ++len) {
                int windows = n - len + 1;
                int correlation = corr[n - len];
                long long same_minus_diff = correlation;
                long long diff = (windows - same_minus_diff + MOD) % MOD * ((MOD + 1LL) / 2) % MOD;
                long long per_window_bit_count = total_bit ? windows : (2 * diff) % MOD;
                bit_sum = (bit_sum + (long long)weight[len] * per_window_bit_count) % MOD;
            }
            answer = (answer + bit_sum * ((1LL << bit) % MOD)) % MOD;
        }
        cout << answer << '\n';
    }
}
