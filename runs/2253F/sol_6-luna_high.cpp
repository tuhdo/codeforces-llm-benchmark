#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
static constexpr int64 INF = (1LL << 62);

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    vector<int64> cost(n + 1);
    for (int i = 1; i <= n; ++i) cin >> cost[i];

    int64 answer = 0;

    // Each component consists of numbers base * 2^a * 3^b,
    // where base is coprime to 6. Its bad sets are local grid patterns:
    // (a,b), (a+1,b), (a+2,b), (a,b+1).
    for (int base = 1; base <= n / 4; ++base) {
        if (base % 2 == 0 || base % 3 == 0) continue;

        vector<int> powers2{1}, powers3{1};
        while (base * 1LL * powers2.back() * 2 <= n)
            powers2.push_back(powers2.back() * 2);
        while (base * 1LL * powers3.back() * 3 <= n)
            powers3.push_back(powers3.back() * 3);

        const int width = static_cast<int>(powers2.size());
        const int fullMask = (1 << width) - 1;
        const int stateCount = 1 << width;

        // A 1 bit means that the corresponding number is kept. The mask
        // stores the last 'width' choices in row-major order.
        vector<int64> dp(stateCount, INF), nextDp(stateCount, INF);
        dp[0] = 0;

        for (int row = 0; row < static_cast<int>(powers3.size()); ++row) {
            for (int col = 0; col < width; ++col) {
                fill(nextDp.begin(), nextDp.end(), INF);
                const int64 value = base * 1LL * powers2[col] * powers3[row];
                const int64 removeCost = (value <= n ? cost[value] : 0);

                for (int mask = 0; mask < stateCount; ++mask) {
                    if (dp[mask] == INF) continue;

                    bool canKeep = true;
                    if (row > 0 && col + 2 < width) {
                        const int lastThree = (mask >> (width - 3)) & 7;
                        if (lastThree == 7) canKeep = false;
                    }

                    if (canKeep) {
                        const int keptMask = ((mask << 1) & fullMask) | 1;
                        nextDp[keptMask] = min(nextDp[keptMask], dp[mask]);
                    }

                    const int removedMask = (mask << 1) & fullMask;
                    nextDp[removedMask] = min(nextDp[removedMask], dp[mask] + removeCost);
                }
                dp.swap(nextDp);
            }
        }

        answer += *min_element(dp.begin(), dp.end());
    }

    cout << answer << '\n';
    return 0;
}
