#include <bits/stdc++.h>
using namespace std;

static vector<int> divisors(int x) {
    vector<int> result;
    for (int d = 1; 1LL * d * d <= x; ++d) {
        if (x % d != 0) continue;
        result.push_back(d);
        if (d * d != x) result.push_back(x / d);
    }
    sort(result.begin(), result.end());
    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, a, b;
    cin >> n >> a >> b;

    const int g = gcd(a, b);
    a /= g;
    b /= g;

    const vector<int> da = divisors(a);
    const vector<int> db = divisors(b);
    const int sa = static_cast<int>(da.size());
    const int sb = static_cast<int>(db.size());

    // nextA[i][k] is the divisor index of da[i] / da[k].
    vector<vector<int>> nextA(sa), nextB(sb);
    vector<vector<int>> removableA(sa), removableB(sb);

    for (int i = 0; i < sa; ++i) {
        for (int k = 0; k <= i; ++k) {
            if (da[i] % da[k] == 0) {
                removableA[i].push_back(k);
                const int quotient = da[i] / da[k];
                nextA[i].push_back(static_cast<int>(
                    lower_bound(da.begin(), da.end(), quotient) - da.begin()));
            }
        }
    }
    for (int i = 0; i < sb; ++i) {
        for (int k = 0; k <= i; ++k) {
            if (db[i] % db[k] == 0) {
                removableB[i].push_back(k);
                const int quotient = db[i] / db[k];
                nextB[i].push_back(static_cast<int>(
                    lower_bound(db.begin(), db.end(), quotient) - db.begin()));
            }
        }
    }

    constexpr int INF = 1'000'000'007;
    vector<vector<int>> dp(sa, vector<int>(sb, -1));

    // Divisors are sorted, so every non-trivial transition goes to a
    // strictly smaller pair of indices. Compute the recurrence lazily.
    auto solve = [&](auto&& self, int i, int j) -> int {
        int& answer = dp[i][j];
        if (answer != -1) return answer;
        answer = INF;
        for (int p = 0; p < static_cast<int>(removableA[i].size()); ++p) {
            const int k = removableA[i][p];
            for (int q = 0; q < static_cast<int>(removableB[j].size()); ++q) {
                const int l = removableB[j][q];
                if (k == 0 && l == 0) continue;
                answer = min(answer,
                             max(da[k], db[l]) +
                                 self(self, nextA[i][p], nextB[j][q]));
            }
        }
        return answer;
    };

    dp[0][0] = 0;
    cout << solve(solve, sa - 1, sb - 1) << '\n';
    return 0;
}
