#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

constexpr int MOD = 1'000'000'007;
constexpr int MAX_N = 500'000;

int mod_pow(int base, int exponent) {
    int result = 1;
    while (exponent > 0) {
        if (exponent & 1) result = static_cast<int64>(result) * base % MOD;
        base = static_cast<int64>(base) * base % MOD;
        exponent >>= 1;
    }
    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<int> fact(MAX_N + 1), inv_fact(MAX_N + 1);
    fact[0] = 1;
    for (int i = 1; i <= MAX_N; ++i) {
        fact[i] = static_cast<int64>(fact[i - 1]) * i % MOD;
    }
    inv_fact[MAX_N] = mod_pow(fact[MAX_N], MOD - 2);
    for (int i = MAX_N; i >= 1; --i) {
        inv_fact[i - 1] = static_cast<int64>(inv_fact[i]) * i % MOD;
    }

    auto choose = [&](int n, int k) {
        return static_cast<int>(static_cast<int64>(fact[n]) * inv_fact[k] % MOD * inv_fact[n - k] % MOD);
    };

    int test_count;
    cin >> test_count;
    while (test_count--) {
        int n;
        cin >> n;
        vector<int64> a(n);
        for (int64 &value : a) cin >> value;

        int answer = 1;
        bool valid = true;
        vector<pair<int, int>> segments = {{0, n - 1}};

        while (!segments.empty() && valid) {
            auto [left, right] = segments.back();
            segments.pop_back();

            int root = -1;
            int i = left, j = right;
            while (i <= j) {
                if (a[i] == static_cast<int64>(i - left + 1) * (right - i + 1)) {
                    root = i;
                    break;
                }
                ++i;

                if (i <= j && a[j] == static_cast<int64>(j - left + 1) * (right - j + 1)) {
                    root = j;
                    break;
                }
                --j;
            }

            if (root == -1) {
                valid = false;
                break;
            }

            answer = static_cast<int64>(answer) * choose(right - left, root - left) % MOD;
            if (left < root) segments.emplace_back(left, root - 1);
            if (root < right) segments.emplace_back(root + 1, right);
        }

        cout << (valid ? answer : 0) << '\n';
    }
}
