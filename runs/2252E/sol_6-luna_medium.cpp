#include <bits/stdc++.h>
using namespace std;

static constexpr int MOD = 1'000'000'007;

// Comparison states: 0 = equal, 1 = less, 2 = greater.
static int advance_cmp(int state, int x, int y) {
    if (x == y) return state;
    return x < y ? 1 : 2;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        unsigned long long n;
        cin >> n;

        // State: carry, previous b bit, cmp(a,b), cmp(b,c), cmp(a,n), cmp(c,n),
        // and whether a has a set bit. Process low to high; each newly
        // differing bit overwrites a comparison, leaving the most
        // significant differing bit at the end.
        long long dp[2][2][3][3][3][3][2]{};
        dp[0][0][0][0][0][0][0] = 1;

        for (int i = 0; i <= 61; ++i) {
            long long ndp[2][2][3][3][3][3][2]{};
            int nb = (i < 64 ? int((n >> i) & 1ULL) : 0);
            for (int carry = 0; carry < 2; ++carry)
                for (int prevB = 0; prevB < 2; ++prevB)
                    for (int ab = 0; ab < 3; ++ab)
                        for (int bc = 0; bc < 3; ++bc)
                            for (int an = 0; an < 3; ++an)
                                for (int cn = 0; cn < 3; ++cn)
                                for (int positive = 0; positive < 2; ++positive) {
                                    long long ways = dp[carry][prevB][ab][bc][an][cn][positive];
                                    if (!ways) continue;
                                    for (int a = 0; a <= 1; ++a)
                                        for (int b = 0; b <= 1; ++b)
                                            for (int c = 0; c <= 1; ++c) {
                                                if (i == 61 && (a || b || c)) continue;
                                                if ((a + b + c) & 1) continue;

                                                int sum = a + c + carry;
                                                int targetBit = prevB;
                                                if ((sum & 1) != targetBit) continue;
                                                int nextCarry = sum >> 1;

                                                int nab = advance_cmp(ab, a, b);
                                                int nbc = advance_cmp(bc, b, c);
                                                int nan = advance_cmp(an, a, nb);
                                                int ncn = advance_cmp(cn, c, nb);
                                                int np = positive || a;
                                                auto &slot = ndp[nextCarry][b][nab][nbc][nan][ncn][np];
                                                slot += ways;
                                                if (slot >= MOD) slot -= MOD;
                                            }
                                }
            memcpy(dp, ndp, sizeof(dp));
        }

        long long answer = 0;
        for (int prevB = 0; prevB < 2; ++prevB)
                for (int an = 0; an <= 1; ++an)
                for (int cn = 0; cn <= 1; ++cn) {
                    // a < b < c, both endpoints <= n; final carry vanishes.
                    answer += dp[0][prevB][1][1][an][cn][1];
                    if (answer >= MOD) answer -= MOD;
                }
        cout << answer % MOD << '\n';
    }
    return 0;
}
