#include <bits/stdc++.h>
using namespace std;

static constexpr int MOD = 1'000'000'007;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        unsigned long long n;
        cin >> n;

        // Comparison values: 0 = equal so far, 1 = below n, 2 = above n.
        // Since bits are processed from least significant to most significant,
        // a differing new bit replaces the comparison from lower bits.
        long long dp[2][2][3][3][3] = {};
        dp[0][0][0][0][0] = 1;

        int bits = 0;
        while ((1ULL << bits) <= n && bits < 63) ++bits;
        // n <= 1e18, so bits is at most 60 and all shifts below are defined.
        for (int i = 0; i < bits; ++i) {
            int nb = (n >> i) & 1ULL;
            long long ndp[2][2][3][3][3] = {};
            for (int carry = 0; carry <= 1; ++carry) {
                for (int prev_b = 0; prev_b <= 1; ++prev_b) {
                    for (int ca = 0; ca < 3; ++ca)
                    for (int cb = 0; cb < 3; ++cb)
                    for (int cc = 0; cc < 3; ++cc) {
                        long long ways = dp[carry][prev_b][ca][cb][cc];
                        if (!ways) continue;
                        for (int a_bit = 0; a_bit <= 1; ++a_bit)
                        for (int b_bit = 0; b_bit <= 1; ++b_bit) {
                            int c_bit = a_bit ^ b_bit;
                            int sum = a_bit + c_bit + carry;
                            if ((sum & 1) != prev_b) continue;
                            int next_carry = sum >> 1;
                            int na = ca, ncb = cb, nc = cc;
                            if (a_bit != nb) na = (a_bit < nb ? 1 : 2);
                            if (b_bit != nb) ncb = (b_bit < nb ? 1 : 2);
                            if (c_bit != nb) nc = (c_bit < nb ? 1 : 2);
                            long long &cell = ndp[next_carry][b_bit][na][ncb][nc];
                            cell += ways;
                            if (cell >= MOD) cell -= MOD;
                        }
                    }
                }
            }
            memcpy(dp, ndp, sizeof(dp));
        }

        long long answer = 0;
        for (int carry = 0; carry <= 1; ++carry) {
            for (int prev_b = 0; prev_b <= 1; ++prev_b) {
                if (carry != prev_b) continue; // Check bit `bits` of a+c = 2b.
                for (int ca = 0; ca <= 1; ++ca)
                for (int cb = 0; cb <= 1; ++cb)
                for (int cc = 0; cc <= 1; ++cc) {
                    answer += dp[carry][prev_b][ca][cb][cc];
                    if (answer >= MOD) answer -= MOD;
                }
            }
        }
        // The DP treats the two AP endpoints as labeled a and c, so every
        // nonzero valid triplet appears in both orientations. The only solution
        // with a == c is (0, 0, 0), which is removed before dividing by two.
        answer = (answer - 1 + MOD) % MOD;
        answer = answer * ((MOD + 1LL) / 2) % MOD;
        cout << answer << '\n';
    }
    return 0;
}
