#include <bits/stdc++.h>
using namespace std;

static constexpr long long MOD = 1'000'000'007LL;
static constexpr long long INV2 = (MOD + 1) / 2;

// Comparison states are encoded as 0 (<), 1 (=), and 2 (>).
int update_comparison(int current, int bit, int limit_bit) {
    if (bit < limit_bit) return 0;
    if (bit > limit_bit) return 2;
    return current;
}

long long count_triplets(unsigned long long n) {
    // dp[carry][comparison of a][comparison of b][comparison of c]
    long long dp[2][3][3][3] = {};
    dp[0][1][1][1] = 1;

    // n < 2^60, but extra zero bits force the final arithmetic carry to vanish.
    for (int bit = 0; bit < 62; ++bit) {
        long long next[2][3][3][3] = {};
        int limit_bit = static_cast<int>((n >> bit) & 1ULL);

        for (int carry = 0; carry <= 1; ++carry) {
            for (int ca = 0; ca < 3; ++ca) {
                for (int cb = 0; cb < 3; ++cb) {
                    for (int cc = 0; cc < 3; ++cc) {
                        long long ways = dp[carry][ca][cb][cc];
                        if (ways == 0) continue;

                        for (int a = 0; a <= 1; ++a) {
                            for (int b = 0; b <= 1; ++b) {
                                for (int c = 0; c <= 1; ++c) {
                                    if ((a ^ b ^ c) != 0) continue;

                                    int sum = a + c + carry - 2 * b;
                                    if (sum < 0 || (sum & 1)) continue;
                                    int next_carry = sum / 2;
                                    if (next_carry > 1) continue;

                                    int na = update_comparison(ca, a, limit_bit);
                                    int nb = update_comparison(cb, b, limit_bit);
                                    int nc = update_comparison(cc, c, limit_bit);
                                    long long &cell = next[next_carry][na][nb][nc];
                                    cell += ways;
                                    if (cell >= MOD) cell -= MOD;
                                }
                            }
                        }
                    }
                }
            }
        }

        memcpy(dp, next, sizeof(dp));
    }

    long long total = 0;
    for (int a_cmp = 0; a_cmp <= 1; ++a_cmp) {
        for (int b_cmp = 0; b_cmp <= 1; ++b_cmp) {
            for (int c_cmp = 0; c_cmp <= 1; ++c_cmp) {
                total += dp[0][a_cmp][b_cmp][c_cmp];
                if (total >= MOD) total -= MOD;
            }
        }
    }

    // The equal triple allowed by XOR is only (0, 0, 0). Every other valid
    // triple occurs once in each orientation (a, b, c) and (c, b, a).
    total = (total - 1 + MOD) % MOD;
    return total * INV2 % MOD;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        unsigned long long n;
        cin >> n;
        cout << count_triplets(n) << '\n';
    }
    return 0;
}
