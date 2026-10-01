#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

vector<int64> get_divisors(int64 x) {
    vector<int64> divisors;
    for (int64 d = 1; d * d <= x; ++d) {
        if (x % d == 0) {
            divisors.push_back(d);
            if (d * d != x) divisors.push_back(x / d);
        }
    }
    sort(divisors.begin(), divisors.end());
    return divisors;
}

struct DivisorData {
    vector<int64> values;
    vector<vector<int>> divisors_of;
    vector<vector<int>> quotient_index;

    explicit DivisorData(int64 x) : values(get_divisors(x)) {
        const int m = static_cast<int>(values.size());
        divisors_of.resize(m);
        quotient_index.assign(m, vector<int>(m, -1));

        for (int i = 0; i < m; ++i) {
            for (int j = 0; j <= i; ++j) {
                if (values[i] % values[j] != 0) continue;
                divisors_of[i].push_back(j);
                int64 quotient = values[i] / values[j];
                quotient_index[i][j] = static_cast<int>(
                    lower_bound(values.begin(), values.end(), quotient) - values.begin());
            }
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int64 n, a, b;
    if (!(cin >> n >> a >> b)) return 0;

    const int64 g = gcd(a, b);
    const int64 x = a / g;
    const int64 y = b / g;

    DivisorData dx(x), dy(y);
    const int sx = static_cast<int>(dx.values.size());
    const int sy = static_cast<int>(dy.values.size());

    // dp[i][j] is the minimum cost for the pair of remaining factors
    // dx.values[i], dy.values[j].
    vector<vector<int64>> dp(sx, vector<int64>(sy, 0));
    for (int i = 0; i < sx; ++i) {
        for (int j = 0; j < sy; ++j) {
            if (i == 0 && j == 0) continue;

            int64 best = LLONG_MAX;
            for (int pi : dx.divisors_of[i]) {
                for (int qj : dy.divisors_of[j]) {
                    if (pi == 0 && qj == 0) continue;
                    int ni = dx.quotient_index[i][pi];
                    int nj = dy.quotient_index[j][qj];
                    best = min(best, max(dx.values[pi], dy.values[qj]) + dp[ni][nj]);
                }
            }
            dp[i][j] = best;
        }
    }

    cout << dp[sx - 1][sy - 1] << '\n';
    return 0;
}
