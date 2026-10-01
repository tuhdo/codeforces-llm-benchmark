#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
constexpr int INF = 1'000'000'007;

vector<int64> divisors(int64 x) {
    vector<int64> result;
    for (int64 d = 1; d * d <= x; ++d) {
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

    int64 n, a, b;
    cin >> n >> a >> b;

    const int64 g = gcd(a, b);
    a /= g;
    b /= g;

    const vector<int64> da = divisors(a);
    const vector<int64> db = divisors(b);
    const int na = static_cast<int>(da.size());
    const int nb = static_cast<int>(db.size());

    // remove_a[i] lists every factor of da[i], together with the index of
    // the quotient after removing it.  The analogous list is made for b.
    auto build_removals = [](const vector<int64>& d) {
        vector<vector<pair<int, int>>> removals(d.size());
        for (int current = 0; current < static_cast<int>(d.size()); ++current) {
            for (int factor = 0; factor <= current; ++factor) {
                if (d[current] % d[factor] != 0) continue;
                int quotient = static_cast<int>(lower_bound(d.begin(), d.end(),
                    d[current] / d[factor]) - d.begin());
                removals[current].push_back({factor, quotient});
            }
        }
        return removals;
    };

    const auto remove_a = build_removals(da);
    const auto remove_b = build_removals(db);
    vector<vector<int>> memo(na, vector<int>(nb, -1));

    auto solve = [&](auto&& self, int ia, int ib) -> int {
        int& answer = memo[ia][ib];
        if (answer != -1) return answer;
        if (ia == 0 && ib == 0) return answer = 0;

        answer = INF;
        for (const auto [fa, qa] : remove_a[ia]) {
            for (const auto [fb, qb] : remove_b[ib]) {
                if (fa == 0 && fb == 0) continue; // no factor is replaced
                answer = min(answer, static_cast<int>(max(da[fa], db[fb]))
                    + self(self, qa, qb));
            }
        }
        return answer;
    };

    cout << solve(solve, na - 1, nb - 1) << '\n';
    return 0;
}
