#include <bits/stdc++.h>
using namespace std;

static constexpr long long MOD = 1'000'000'007LL;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        unsigned long long n;
        cin >> n;

        // State: carry in a+c=2b, previous bit of b,
        // comparison of a and b, comparison of c and n, and a>0.
        // Comparison values are 0 (<), 1 (=), 2 (>).
        long long dp[2][2][3][3][2] = {};
        dp[0][0][1][1][0] = 1;

        // n < 2^60, but two extra positions flush all arithmetic carry.
        for (int bit = 0; bit <= 62; ++bit) {
            long long ndp[2][2][3][3][2] = {};
            int nb = (bit < 64) ? int((n >> bit) & 1ULL) : 0;

            for (int carry = 0; carry < 2; ++carry) {
                for (int prev_b = 0; prev_b < 2; ++prev_b) {
                    for (int ab = 0; ab < 3; ++ab) {
                        for (int cn = 0; cn < 3; ++cn) {
                            for (int positive = 0; positive < 2; ++positive) {
                                long long cur = dp[carry][prev_b][ab][cn][positive];
                                if (!cur) continue;

                                for (int a = 0; a <= 1; ++a) {
                                    for (int b = 0; b <= 1; ++b) {
                                        for (int c = 0; c <= 1; ++c) {
                                            if ((a ^ b ^ c) != 0) continue;

                                            int sum = a + c + carry;
                                            if ((sum & 1) != prev_b) continue;
                                            int next_carry = sum >> 1;

                                            int next_ab = ab;
                                            if (a != b) next_ab = (a < b ? 0 : 2);

                                            int next_cn = cn;
                                            if (c != nb) next_cn = (c < nb ? 0 : 2);

                                            int next_positive = positive | a;
                                            long long &dest = ndp[next_carry][b]
                                                [next_ab][next_cn][next_positive];
                                            dest += cur;
                                            if (dest >= MOD) dest -= MOD;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            memcpy(dp, ndp, sizeof(dp));
        }

        long long answer = 0;
        for (int prev_b = 0; prev_b < 2; ++prev_b) {
            answer += dp[0][prev_b][0][0][1];
            answer += dp[0][prev_b][0][1][1];
        }
        cout << answer % MOD << '\n';
    }
}
