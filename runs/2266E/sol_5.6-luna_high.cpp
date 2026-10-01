#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;

    const int MAX_N = 200000;
    vector<int> smallest_prime_factor(MAX_N + 1);
    for (int i = 2; i <= MAX_N; ++i) {
        if (smallest_prime_factor[i] != 0) {
            continue;
        }
        for (int j = i; j <= MAX_N; j += i) {
            if (smallest_prime_factor[j] == 0) {
                smallest_prime_factor[j] = i;
            }
        }
    }

    while (t--) {
        int n, k;
        cin >> n >> k;

        vector<int> count(n + 1);
        for (int i = 0; i < n; ++i) {
            int x;
            cin >> x;
            ++count[x];
        }

        vector<long long> cost(n + 1, 0);
        for (int x = k + 1; x <= n; ++x) {
            long long best = numeric_limits<long long>::max();
            int remaining = x;
            while (remaining > 1) {
                int p = smallest_prime_factor[remaining];
                best = min(best, 1LL + 1LL * p * cost[x / p]);
                while (remaining % p == 0) {
                    remaining /= p;
                }
            }
            cost[x] = best;
        }

        long long answer = 0;
        for (int x = k + 1; x <= n; ++x) {
            answer += 1LL * count[x] * cost[x];
        }
        cout << answer << '\n';
    }

    return 0;
}
