#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCount;
    cin >> testCount;

    constexpr int MAX_VALUE = 200000;
    vector<int> smallestPrime(MAX_VALUE + 1);
    for (int i = 2; i <= MAX_VALUE; ++i) {
        if (smallestPrime[i] == 0) {
            for (int multiple = i; multiple <= MAX_VALUE; multiple += i) {
                if (smallestPrime[multiple] == 0) {
                    smallestPrime[multiple] = i;
                }
            }
        }
    }

    while (testCount--) {
        int n, k;
        cin >> n >> k;

        vector<int> values(n);
        for (int& value : values) {
            cin >> value;
        }

        vector<long long> cost(n + 1);
        for (int x = k + 1; x <= n; ++x) {
            cost[x] = LLONG_MAX;
            int remaining = x;
            while (remaining > 1) {
                const int p = smallestPrime[remaining];
                cost[x] = min(cost[x], 1 + 1LL * p * cost[x / p]);
                while (remaining % p == 0) {
                    remaining /= p;
                }
            }
        }

        long long answer = 0;
        for (int value : values) {
            answer += cost[value];
        }
        cout << answer << '\n';
    }
}
