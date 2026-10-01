#include <bits/stdc++.h>

using namespace std;

static constexpr long long MOD = 1'000'000'007LL;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCases;
    cin >> testCases;

    while (testCases--) {
        unsigned long long n;
        cin >> n;

        // State: previous t bit, previous w bit, comparison of the processed
        // prefix of c with n (-1, 0, or 1), whether t is nonzero, and whether
        // the highest set bit of t is selected in w.
        long long dp[2][2][3][2][2]{};
        dp[0][0][1][0][0] = 1;

        // n <= 10^18 < 2^60. Bit 60 is processed to reject a possible
        // overflow from w[59] << 1.
        for (int bit = 0; bit <= 60; ++bit) {
            long long next[2][2][3][2][2]{};
            const int nBit = static_cast<int>((n >> bit) & 1ULL);

            for (int previousT = 0; previousT <= 1; ++previousT) {
                for (int previousW = 0; previousW <= 1; ++previousW) {
                    for (int relation = 0; relation < 3; ++relation) {
                        for (int seen = 0; seen <= 1; ++seen) {
                            for (int highestSelected = 0; highestSelected <= 1;
                                 ++highestSelected) {
                                const long long ways =
                                    dp[previousT][previousW][relation][seen]
                                      [highestSelected];
                                if (ways == 0) {
                                    continue;
                                }

                                const int maxT = (bit == 60 ? 0 : 1);
                                for (int tBit = 0; tBit <= maxT; ++tBit) {
                                    if (tBit && previousT) {
                                        continue;
                                    }

                                    const int maxW = tBit;
                                    for (int wBit = 0; wBit <= maxW; ++wBit) {
                                        const int cBit = tBit | previousW;
                                        int nextRelation = relation;
                                        if (cBit != nBit) {
                                            nextRelation = (cBit < nBit ? 0 : 2);
                                        }

                                        const int nextSeen = seen | tBit;
                                        const int nextHighestSelected =
                                            tBit ? wBit : highestSelected;
                                        long long &target =
                                            next[tBit][wBit][nextRelation]
                                                [nextSeen][nextHighestSelected];
                                        target += ways;
                                        if (target >= MOD) {
                                            target -= MOD;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            memcpy(dp, next, sizeof(dp));
        }

        long long answer = 0;
        for (int previousT = 0; previousT <= 1; ++previousT) {
            for (int previousW = 0; previousW <= 1; ++previousW) {
                for (int relation = 0; relation <= 1; ++relation) {
                    answer += dp[previousT][previousW][relation][1][1];
                }
            }
        }
        cout << answer % MOD << '\n';
    }
}
