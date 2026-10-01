#include <bits/stdc++.h>

using namespace std;

using int64 = long long;

constexpr int MOD = 1'000'000'007;
constexpr int MAX_N = 200'000;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<int64> fact(MAX_N + 1, 1);
    for (int i = 1; i <= MAX_N; ++i) {
        fact[i] = fact[i - 1] * i % MOD;
    }

    int test_count;
    cin >> test_count;

    while (test_count--) {
        int n;
        cin >> n;

        vector<int64> a(n);
        for (int64& value : a) {
            cin >> value;
        }

        const int64 maximum = a.back();
        const int64 minimum_internal = a[1];
        const int64 maximum_internal = a[n - 2];

        // The two arithmetic progressions have these lengths.
        const int lower_count = (n - 2) / 2;
        const int upper_count = (n - 1) / 2;

        vector<int64> candidates = {
            minimum_internal,
            maximum - maximum_internal,
        };

        if ((maximum - minimum_internal) % upper_count == 0) {
            candidates.push_back((maximum - minimum_internal) / upper_count);
        }
        if (maximum_internal % lower_count == 0) {
            candidates.push_back(maximum_internal / lower_count);
        }

        sort(candidates.begin(), candidates.end());
        candidates.erase(unique(candidates.begin(), candidates.end()), candidates.end());

        int valid_gaps = 0;
        for (const int64 gap : candidates) {
            if (gap <= 0 || gap >= maximum) {
                continue;
            }

            vector<int64> expected;
            expected.reserve(n - 2);
            bool possible = true;

            for (int k = 1; k <= lower_count; ++k) {
                const int64 value = gap * k;
                if (value <= 0 || value >= maximum) {
                    possible = false;
                    break;
                }
                expected.push_back(value);
            }

            for (int k = 1; possible && k <= upper_count; ++k) {
                const int64 value = maximum - gap * k;
                if (value <= 0 || value >= maximum) {
                    possible = false;
                    break;
                }
                expected.push_back(value);
            }

            if (!possible) {
                continue;
            }

            sort(expected.begin(), expected.end());
            for (int i = 0; i < n - 2; ++i) {
                if (expected[i] != a[i + 1]) {
                    possible = false;
                    break;
                }
            }

            valid_gaps += possible;
        }

        const int64 arrangements = fact[n / 2] * fact[(n - 3) / 2] % MOD;
        cout << arrangements * valid_gaps % MOD << '\n';
    }

    return 0;
}
