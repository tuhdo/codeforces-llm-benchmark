#include <bits/stdc++.h>
using namespace std;

struct DivisorSet {
    vector<int> values;
    // For each divisor, store (chosen factor, quotient's state index).
    vector<vector<pair<int, int>>> sub;
    unordered_map<int, int> index;
    vector<int> prime_factors;

    explicit DivisorSet(int value) {
        vector<pair<int, int>> factors;
        int remaining = value;
        for (int p = 2; 1LL * p * p <= remaining; ++p) {
            if (remaining % p != 0) continue;
            int exponent = 0;
            do {
                remaining /= p;
                ++exponent;
            } while (remaining % p == 0);
            factors.push_back({p, exponent});
            prime_factors.push_back(p);
        }
        if (remaining > 1) {
            factors.push_back({remaining, 1});
            prime_factors.push_back(remaining);
        }

        values = {1};
        for (auto [p, exponent] : factors) {
            int old_size = static_cast<int>(values.size());
            int power = 1;
            for (int e = 1; e <= exponent; ++e) {
                power *= p;
                for (int i = 0; i < old_size; ++i) {
                    values.push_back(values[i] * power);
                }
            }
        }
        sort(values.begin(), values.end());

        index.reserve(values.size() * 2);
        for (int i = 0; i < static_cast<int>(values.size()); ++i) {
            index.emplace(values[i], i);
        }

        // For every divisor d, store the indices of all divisors of d.
        sub.resize(values.size());
        for (int i = 0; i < static_cast<int>(values.size()); ++i) {
            for (int j = 0; j <= i; ++j) {
                if (values[i] % values[j] == 0) {
                    sub[i].push_back({values[j], index.at(values[i] / values[j])});
                }
            }
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, a, b;
    cin >> n >> a >> b;

    int common = gcd(a, b);
    a /= common;
    b /= common;

    DivisorSet left(a), right(b);
    const int left_count = static_cast<int>(left.values.size());
    const int right_count = static_cast<int>(right.values.size());
    const long long INF = (1LL << 60);

    vector<vector<long long>> memo(left_count,
                                   vector<long long>(right_count, -1));

    function<long long(int, int)> solve = [&](int left_id, int right_id) -> long long {
        long long &answer = memo[left_id][right_id];
        if (answer != -1) return answer;

        int x = left.values[left_id];
        int y = right.values[right_id];
        if (x == 1 && y == 1) return answer = 0;

        answer = INF;

        // A normalized factorization has at least one prime factor in each
        // pair of blocks. First take a prime block from x and any divisor of y.
        for (int p : left.prime_factors) {
            if (x % p != 0) continue;
            int next_left = left.index.at(x / p);
            for (auto [q, next_right] : right.sub[right_id]) {
                answer = min(answer, static_cast<long long>(max(p, q)) +
                                         solve(next_left, next_right));
            }
        }

        // Then take a prime block from y and any non-prime divisor of x.
        // Prime divisors of x were already handled above, so this avoids
        // processing the same prime/prime pair twice.
        for (int q : right.prime_factors) {
            if (y % q != 0) continue;
            int next_right = right.index.at(y / q);
            for (auto [p, next_left] : left.sub[left_id]) {
                if (p > 1 && binary_search(left.prime_factors.begin(),
                                           left.prime_factors.end(), p)) {
                    continue;
                }
                answer = min(answer, static_cast<long long>(max(p, q)) +
                                         solve(next_left, next_right));
            }
        }

        return answer;
    };

    cout << solve(left.index.at(a), right.index.at(b)) << '\n';
    return 0;
}
