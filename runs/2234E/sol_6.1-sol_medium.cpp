#include <bits/stdc++.h>
using namespace std;

constexpr long long MOD = 1000000007;
constexpr int MAX_N = 500000;

long long power(long long base, long long exponent) {
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

    vector<long long> factorial(MAX_N + 1), inverseFactorial(MAX_N + 1);
    factorial[0] = 1;
    for (int i = 1; i <= MAX_N; ++i) {
        factorial[i] = factorial[i - 1] * i % MOD;
    }
    inverseFactorial[MAX_N] = power(factorial[MAX_N], MOD - 2);
    for (int i = MAX_N; i > 0; --i) {
        inverseFactorial[i - 1] = inverseFactorial[i] * i % MOD;
    }

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<long long> a(n + 2);
        vector<int> previous(n + 2), next(n + 2);
        vector<int> leftSize(n + 2), rightSize(n + 2);
        vector<unsigned char> removed(n + 2);
        vector<int> pending;
        pending.reserve(3 * n);
        for (int i = 1; i <= n; ++i) {
            cin >> a[i];
            previous[i] = i - 1;
            next[i] = i + 1;
            if (a[i] == 1) pending.push_back(i);
        }

        auto ready = [&](int i) {
            return i >= 1 && i <= n && !removed[i] &&
                   1LL * (leftSize[i] + 1) * (rightSize[i] + 1) == a[i];
        };

        long long answer = 1;
        int processed = 0;
        for (size_t head = 0; head < pending.size(); ++head) {
            int i = pending[head];
            if (!ready(i)) continue;

            int l = leftSize[i], r = rightSize[i];
            answer = answer * factorial[l + r] % MOD;
            answer = answer * inverseFactorial[l] % MOD;
            answer = answer * inverseFactorial[r] % MOD;
            removed[i] = 1;
            ++processed;

            // The new completed subtree fills the gap between these active nodes.
            int before = previous[i], after = next[i];
            int size = l + r + 1;
            if (before >= 1) {
                next[before] = after;
                rightSize[before] = size;
            }
            if (after <= n) {
                previous[after] = before;
                leftSize[after] = size;
            }
            if (ready(before)) pending.push_back(before);
            if (ready(after)) pending.push_back(after);
        }

        cout << (processed == n ? answer : 0) << '\n';
    }
}
