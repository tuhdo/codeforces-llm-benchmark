#include <bits/stdc++.h>
using namespace std;

static constexpr int MOD = 1'000'000'007;
static constexpr int INV2 = 500'000'004;

int update_comparison(int comparison, int bit, int limit_bit) {
    if (bit < limit_bit) return 0; // less
    if (bit > limit_bit) return 2; // greater
    return comparison;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        unsigned long long n;
        cin >> n;

        // dp[previous AND bit][comparison of a with n][comparison of c with n]
        // Comparisons are encoded as 0 = less, 1 = equal, 2 = greater.
        int dp[2][3][3] = {};
        dp[0][1][1] = 1;

        // n < 2^60. The extra bit checks the relation from bit 59.
        for (int pos = 0; pos <= 60; ++pos) {
            int limit_bit = (n >> pos) & 1ULL;
            int next_dp[2][3][3] = {};

            for (int previous_and = 0; previous_and <= 1; ++previous_and) {
                for (int cmp_a = 0; cmp_a < 3; ++cmp_a) {
                    for (int cmp_c = 0; cmp_c < 3; ++cmp_c) {
                        int ways = dp[previous_and][cmp_a][cmp_c];
                        if (ways == 0) continue;

                        for (int bit_a = 0; bit_a <= 1; ++bit_a) {
                            for (int bit_c = 0; bit_c <= 1; ++bit_c) {
                                if ((bit_a ^ bit_c) != previous_and) continue;

                                int next_and = bit_a & bit_c;
                                int next_cmp_a = update_comparison(cmp_a, bit_a, limit_bit);
                                int next_cmp_c = update_comparison(cmp_c, bit_c, limit_bit);
                                int &dest = next_dp[next_and][next_cmp_a][next_cmp_c];
                                dest += ways;
                                if (dest >= MOD) dest -= MOD;
                            }
                        }
                    }
                }
            }

            memcpy(dp, next_dp, sizeof(dp));
        }

        int ordered_pairs = 0;
        // The relation at bit 61 requires the final AND bit to be zero.
        for (int cmp_a = 0; cmp_a <= 1; ++cmp_a) {
            for (int cmp_c = 0; cmp_c <= 1; ++cmp_c) {
                ordered_pairs += dp[0][cmp_a][cmp_c];
                if (ordered_pairs >= MOD) ordered_pairs -= MOD;
            }
        }

        // Remove (0, 0); every remaining pair has distinct outer terms and
        // each triplet is represented by the two orders of those terms.
        ordered_pairs = (ordered_pairs + MOD - 1) % MOD;
        cout << (1LL * ordered_pairs * INV2) % MOD << '\n';
    }

    return 0;
}
