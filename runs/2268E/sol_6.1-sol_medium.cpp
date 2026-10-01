#include <bits/stdc++.h>
using namespace std;

constexpr int MOD = 998244353;
constexpr int MAX_N = 200000;

int power(int a, int e) {
    int result = 1;
    for (; e; e >>= 1, a = (long long)a * a % MOD)
        if (e & 1) result = (long long)result * a % MOD;
    return result;
}

struct NTT {
    int size;
    vector<int> rev, roots;

    explicit NTT(int size) : size(size), rev(size), roots(size) {
        for (int i = 1; i < size; ++i)
            rev[i] = (rev[i >> 1] >> 1) | ((i & 1) * (size >> 1));
        roots[1] = 1;
        for (int len = 2; len < size; len <<= 1) {
            int z = power(3, (MOD - 1) / (2 * len));
            for (int i = len / 2; i < len; ++i) {
                roots[2 * i] = roots[i];
                roots[2 * i + 1] = (long long)roots[i] * z % MOD;
            }
        }
    }

    void transform(vector<int>& a) const {
        for (int i = 0; i < size; ++i)
            if (i < rev[i]) swap(a[i], a[rev[i]]);
        for (int len = 1; len < size; len <<= 1) {
            for (int start = 0; start < size; start += 2 * len) {
                for (int j = 0; j < len; ++j) {
                    int u = a[start + j];
                    int v = (long long)a[start + j + len] * roots[len + j] % MOD;
                    int sum = u + v;
                    if (sum >= MOD) sum -= MOD;
                    int difference = u - v;
                    if (difference < 0) difference += MOD;
                    a[start + j] = sum;
                    a[start + j + len] = difference;
                }
            }
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<int> inverse(MAX_N + 2), catalan(MAX_N + 1);
    inverse[1] = 1;
    for (int i = 2; i <= MAX_N + 1; ++i)
        inverse[i] = MOD - (long long)(MOD / i) * inverse[MOD % i] % MOD;
    catalan[0] = 1;
    for (int i = 1; i <= MAX_N; ++i)
        catalan[i] = (long long)catalan[i - 1] * (4 * i - 2) % MOD * inverse[i + 1] % MOD;

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        vector<int> prefix(n + 1);
        int present_bits = 0;
        for (int i = 1; i <= n; ++i) {
            int a;
            cin >> a;
            prefix[i] = prefix[i - 1] ^ a;
            present_bits |= a;
        }
        vector<int> weight(n + 1);
        for (int k = 1; k < n; ++k)
            weight[k] = (long long)catalan[k] * catalan[n - k] % MOD;

        // Every proper subtree corresponds to one edge cut.
        if (n <= 64) {
            int answer = 0;
            for (int l = 0; l < n; ++l) {
                for (int r = l + 1; r <= n; ++r) {
                    int x = prefix[r] ^ prefix[l];
                    int contribution = (long long)weight[r - l] * (x + (x ^ prefix[n])) % MOD;
                    answer += contribution;
                    if (answer >= MOD) answer -= MOD;
                }
            }
            cout << answer << '\n';
            continue;
        }

        int answer = (long long)prefix[n] * (n - 1) % MOD * catalan[n] % MOD;
        int needed_bits = present_bits & ~prefix[n];
        if (needed_bits == 0) {
            cout << answer << '\n';
            continue;
        }

        int size = 1;
        while (size < 2 * n) size <<= 1;
        NTT ntt(size);
        vector<int> kernel(size), signs(size);
        int base = 0;
        for (int k = 1; k < n; ++k) {
            kernel[k] = kernel[size - k] = weight[k];
            base = (base + (long long)(n + 1 - k) * weight[k]) % MOD;
        }
        ntt.transform(kernel);
        int normalization = power(2 * size, MOD - 2);

        for (int bit = 0; bit < 18; ++bit) {
            if (!(needed_bits & (1 << bit))) continue;
            fill(signs.begin(), signs.end(), 0);
            for (int i = 0; i <= n; ++i)
                signs[i] = (prefix[i] & (1 << bit)) ? MOD - 1 : 1;
            ntt.transform(signs);

            // Fourier orthogonality gives twice the weighted correlation,
            // multiplied by the transform size. The kernel is symmetric.
            int dot = (long long)kernel[0] * signs[0] % MOD * signs[0] % MOD;
            for (int i = 1; i < size / 2; ++i) {
                int term = (long long)signs[i] * signs[size - i] % MOD * kernel[i] % MOD;
                dot = (dot + 2LL * term) % MOD;
            }
            dot = (dot + (long long)kernel[size / 2] * signs[size / 2] % MOD * signs[size / 2]) % MOD;
            int correlation = (long long)dot * normalization % MOD;
            int contribution = base - correlation;
            if (contribution < 0) contribution += MOD;
            answer = (answer + (long long)(1 << bit) * contribution) % MOD;
        }
        cout << answer << '\n';
    }
}
