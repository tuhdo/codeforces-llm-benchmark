#include <bits/stdc++.h>
using namespace std;

static constexpr int MOD = 1'000'000'007;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        long long n;
        cin >> n;

        // side describes whether the previous bit of p was assigned to a
        // (1), to c (2), or was zero (0). Comparisons are for the processed
        // low bits and use 0 = less, 1 = equal, 2 = greater.
        int dp[3][3][3] = {};
        dp[0][1][1] = 1;

        for (int bit = 0; bit <= 60; ++bit) {
            int ndp[3][3][3] = {};
            const int nbit = static_cast<int>((n >> bit) & 1LL);

            for (int prevSide = 0; prevSide < 3; ++prevSide) {
                for (int cmpA = 0; cmpA < 3; ++cmpA) {
                    for (int cmpC = 0; cmpC < 3; ++cmpC) {
                        const int ways = dp[prevSide][cmpA][cmpC];
                        if (ways == 0) continue;

                        // p has no adjacent set bits. Bit 60 is forced to
                        // zero; it is processed to account for the shift 2p.
                        const int maxPBit = (bit == 60 ? 0 : 1);
                        for (int pBit = 0; pBit <= maxPBit; ++pBit) {
                            if (pBit && prevSide != 0) continue;

                            const int firstSide = pBit ? 1 : 0;
                            const int lastSide = pBit ? 2 : 0;
                            for (int side = firstSide; side <= lastSide; ++side) {
                                const int aBit = pBit || (prevSide == 1);
                                const int cBit = pBit || (prevSide == 2);

                                // A more significant bit overrides the
                                // comparison of all lower bits.
                                const int nextCmpA = (aBit == nbit) ? cmpA : (aBit < nbit ? 0 : 2);
                                const int nextCmpC = (cBit == nbit) ? cmpC : (cBit < nbit ? 0 : 2);
                                int &cell = ndp[side][nextCmpA][nextCmpC];
                                cell += ways;
                                if (cell >= MOD) cell -= MOD;
                            }
                        }
                    }
                }
            }
            memcpy(dp, ndp, sizeof(dp));
        }

        int orientedPairs = 0;
        for (int side = 0; side < 3; ++side) {
            for (int cmpA = 0; cmpA <= 1; ++cmpA) {
                for (int cmpC = 0; cmpC <= 1; ++cmpC) {
                    orientedPairs += dp[side][cmpA][cmpC];
                    if (orientedPairs >= MOD) orientedPairs -= MOD;
                }
            }
        }

        // Remove p = 0 (the sole all-zero construction), then identify the
        // two assignments that merely swap the endpoints of each triplet.
        int validOrientedPairs = orientedPairs - 1;
        if (validOrientedPairs < 0) validOrientedPairs += MOD;
        const long long answer = 1LL * validOrientedPairs * ((MOD + 1) / 2) % MOD;
        cout << answer << '\n';
    }
    return 0;
}
