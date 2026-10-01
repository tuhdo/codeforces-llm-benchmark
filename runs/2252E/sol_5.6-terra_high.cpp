#include <array>
#include <cstdint>
#include <iostream>

using namespace std;

constexpr long long MOD = 1'000'000'007LL;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        unsigned long long n;
        cin >> n;

        // State: previous bit of q, borrow in n-c, borrow in b-a, and
        // whether q is non-zero.  Here b=2q, while a is reconstructed
        // bit by bit from q (see the derivation in the accompanying logic).
        long long dp[2][2][2][2] = {};
        dp[0][0][0][0] = 1;

        // q may use bit 60; the two extra positions flush all shifted bits.
        for (int bit = 0; bit <= 62; ++bit) {
            long long next[2][2][2][2] = {};
            int nbit = bit < 64 ? static_cast<int>((n >> bit) & 1ULL) : 0;

            for (int previous_q = 0; previous_q <= 1; ++previous_q) {
                for (int borrow_c = 0; borrow_c <= 1; ++borrow_c) {
                    for (int borrow_difference = 0; borrow_difference <= 1; ++borrow_difference) {
                        for (int nonzero_q = 0; nonzero_q <= 1; ++nonzero_q) {
                            long long ways = dp[previous_q][borrow_c][borrow_difference][nonzero_q];
                            if (ways == 0) continue;

                            // If the preceding q-bit is 1, q's current bit
                            // must be 0 and a's current bit is free.  Otherwise
                            // a's current bit equals q's current bit.
                            for (int qbit = 0; qbit <= (bit <= 60 ? 1 : 0); ++qbit) {
                                if (previous_q && qbit) continue;
                                int choices = previous_q ? 2 : 1;
                                for (int choice = 0; choice < choices; ++choice) {
                                    int abit = previous_q ? choice : qbit;
                                    int bbit = previous_q;
                                    int cbit = abit ^ bbit;

                                    int next_borrow_c = (nbit - borrow_c < cbit);
                                    int next_borrow_difference = (bbit - borrow_difference < abit);
                                    int next_nonzero_q = nonzero_q || qbit;

                                    long long &cell = next[qbit][next_borrow_c]
                                                           [next_borrow_difference][next_nonzero_q];
                                    cell += ways;
                                    if (cell >= MOD) cell -= MOD;
                                }
                            }
                        }
                    }
                }
            }
            for (int a = 0; a < 2; ++a)
                for (int b = 0; b < 2; ++b)
                    for (int c = 0; c < 2; ++c)
                        for (int d = 0; d < 2; ++d)
                            dp[a][b][c][d] = next[a][b][c][d];
        }

        cout << dp[0][0][0][1] << '\n';
    }
}
