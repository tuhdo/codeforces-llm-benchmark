#include <bits/stdc++.h>
using namespace std;

constexpr long long MOD = 1000000007;
constexpr int MAX_N = 500000;

long long mod_pow(long long base, long long exponent) {
    long long result = 1;
    while (exponent > 0) {
        if (exponent & 1) result = result * base % MOD;
        base = base * base % MOD;
        exponent >>= 1;
    }
    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<long long> factorial(MAX_N + 1), inverse_factorial(MAX_N + 1);
    factorial[0] = 1;
    for (int i = 1; i <= MAX_N; ++i) {
        factorial[i] = factorial[i - 1] * i % MOD;
    }
    inverse_factorial[MAX_N] = mod_pow(factorial[MAX_N], MOD - 2);
    for (int i = MAX_N; i > 0; --i) {
        inverse_factorial[i - 1] = inverse_factorial[i] * i % MOD;
    }

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        vector<long long> a(n);
        long long sum = 0;
        for (long long& value : a) {
            cin >> value;
            sum += value;
        }
        if (sum != 1LL * n * (n + 1) / 2) {
            cout << 0 << '\n';
            continue;
        }

        long long answer = 1;
        vector<pair<int, int>> pending;
        pending.emplace_back(0, n - 1);
        while (!pending.empty()) {
            auto [left, right] = pending.back();
            pending.pop_back();

            int root = -1;
            // Charge the scan to the smaller child, avoiding quadratic work
            // even when the Cartesian tree is a chain.
            for (int lo = left, hi = right; lo <= hi; ++lo, --hi) {
                if (a[lo] == 1LL * (lo - left + 1) * (right - lo + 1)) {
                    root = lo;
                    break;
                }
                if (a[hi] == 1LL * (hi - left + 1) * (right - hi + 1)) {
                    root = hi;
                    break;
                }
            }
            if (root == -1) {
                answer = 0;
                break;
            }

            int left_size = root - left;
            int right_size = right - root;
            // Choose the labels belonging to the left child. The root must
            // receive the smallest label of this segment.
            answer = answer * factorial[left_size + right_size] % MOD;
            answer = answer * inverse_factorial[left_size] % MOD;
            answer = answer * inverse_factorial[right_size] % MOD;

            if (left_size > 0) pending.emplace_back(left, root - 1);
            if (right_size > 0) pending.emplace_back(root + 1, right);
        }
        cout << answer << '\n';
    }
    return 0;
}
