#include <array>
#include <iostream>

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

        // dp[previous bit type][highest chosen x-bit is R][borrow]
        // previous bit type: 0 = no x-bit, 1 = Z, 2 = R
        long long dp[3][2][2] = {};
        dp[0][0][0] = 1;

        auto advance = [&](long long next[3][2][2], int state, int top_r,
                           int borrow, int output_bit, int next_state,
                           int next_top_r, int n_bit) {
            const int next_borrow = output_bit + borrow > n_bit;
            long long& cell = next[next_state][next_top_r][next_borrow];
            cell += dp[state][top_r][borrow];
            if (cell >= MOD) {
                cell -= MOD;
            }
        };

        // n < 2^60. Bit 60 is included to reject every pattern whose
        // highest selected bit is too large, then bit 61 flushes an R bit.
        for (int bit = 0; bit <= 61; ++bit) {
            long long next[3][2][2] = {};
            const int n_bit = (n >> bit) & 1ULL;

            for (int state = 0; state < 3; ++state) {
                for (int top_r = 0; top_r < 2; ++top_r) {
                    for (int borrow = 0; borrow < 2; ++borrow) {
                        if (dp[state][top_r][borrow] == 0) {
                            continue;
                        }

                        if (state == 0 && bit <= 60) {
                            // Leave x_bit equal to 0.
                            advance(next, state, top_r, borrow, 0, 0, top_r,
                                    n_bit);
                            // Set x_bit and put it in z.
                            advance(next, state, top_r, borrow, 1, 1, 0,
                                    n_bit);
                            // Set x_bit and put it in r = x - z.
                            advance(next, state, top_r, borrow, 1, 2, 1,
                                    n_bit);
                        } else if (state == 0) {
                            // The final flushing bit has no x_bit choice.
                            advance(next, state, top_r, borrow, 0, 0, top_r,
                                    n_bit);
                        } else if (state == 1) {
                            // Adjacent set bits in x are forbidden.
                            advance(next, state, top_r, borrow, 0, 0, top_r,
                                    n_bit);
                        } else {
                            // An R at bit - 1 also contributes to this bit.
                            advance(next, state, top_r, borrow, 1, 0, top_r,
                                    n_bit);
                        }
                    }
                }
            }

            for (int state = 0; state < 3; ++state) {
                for (int top_r = 0; top_r < 2; ++top_r) {
                    for (int borrow = 0; borrow < 2; ++borrow) {
                        dp[state][top_r][borrow] = next[state][top_r][borrow];
                    }
                }
            }
        }

        long long answer = 0;
        for (int state = 0; state < 3; ++state) {
            answer += dp[state][1][0];
            if (answer >= MOD) {
                answer -= MOD;
            }
        }
        cout << answer << '\n';
    }
}
