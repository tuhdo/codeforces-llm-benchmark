#include <bits/stdc++.h>
using namespace std;

struct DivisorData {
    vector<int> value{1};
    vector<int> prime, stride, radix;
    vector<vector<int>> divisors;
    vector<vector<int>> removable;

    explicit DivisorData(int number) {
        for (int p = 2; 1LL * p * p <= number; ++p) {
            if (number % p != 0) continue;
            int exponent = 0;
            while (number % p == 0) {
                number /= p;
                ++exponent;
            }
            add_prime(p, exponent);
        }
        if (number > 1) add_prime(number, 1);

        int size = value.size();
        divisors.resize(size);
        removable.resize(size);
        for (int i = 0; i < size; ++i) {
            divisors[i].push_back(0);
            for (int t = 0; t < (int)prime.size(); ++t) {
                int exponent = i / stride[t] % radix[t];
                if (exponent > 0) removable[i].push_back(t);
                int previous_size = divisors[i].size();
                for (int e = 1; e <= exponent; ++e) {
                    for (int k = 0; k < previous_size; ++k) {
                        divisors[i].push_back(divisors[i][k] + e * stride[t]);
                    }
                }
            }
        }
    }

    void add_prime(int p, int exponent) {
        int previous_size = value.size();
        prime.push_back(p);
        stride.push_back(previous_size);
        radix.push_back(exponent + 1);
        int power = 1;
        for (int e = 1; e <= exponent; ++e) {
            power *= p;
            for (int i = 0; i < previous_size; ++i) {
                value.push_back(value[i] * power);
            }
        }
    }
};

int shortest_path(int a, int b) {
    int common = gcd(a, b);
    DivisorData left(a / common), right(b / common);
    int rows = left.value.size(), columns = right.value.size();
    vector<int> dp(rows * columns, INT_MAX);
    dp[0] = 0;

    // Indices encode prime exponents in mixed radix. If k divides i,
    // the quotient has index i-k, so every transition uses an earlier state.
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            if (i == 0 && j == 0) continue;
            int best = INT_MAX;
            for (int t : left.removable[i]) {
                int previous_row = (i - left.stride[t]) * columns;
                int p = left.prime[t];
                for (int k : right.divisors[j]) {
                    best = min(best, dp[previous_row + j - k]
                                     + max(p, right.value[k]));
                }
            }
            for (int t : right.removable[j]) {
                int previous_column = j - right.stride[t];
                int p = right.prime[t];
                for (int k : left.divisors[i]) {
                    best = min(best, dp[(i - k) * columns + previous_column]
                                     + max(left.value[k], p));
                }
            }
            dp[i * columns + j] = best;
        }
    }
    return dp.back();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, a, b;
    cin >> n >> a >> b;
    cout << shortest_path(a, b) << '\n';
    return 0;
}
