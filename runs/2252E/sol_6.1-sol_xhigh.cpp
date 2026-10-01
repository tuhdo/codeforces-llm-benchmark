#include <bits/stdc++.h>
using namespace std;

constexpr int MOD = 1'000'000'007;

int count_triplets(unsigned long long n) {
    // b = a ^ c = 2 * (a & c). From high to low, the paired bits
    // of (a, c) are 00 or a two-bit block: 01/10 followed by 11.
    // Since a < c, the first block contributes 11 to c. Each later
    // run of ones has a unique parsing: pair its ones, using the
    // preceding zero for an odd run. Thus each c whose initial run
    // of ones has even length gives exactly one valid triplet.

    // States: leading zeros, odd initial run, even initial run,
    // and an even initial run that has already ended.
    constexpr int next_state[4][2] = {
        {0, 1},
        {-1, 2},
        {3, 1},
        {3, 3}
    };

    int dp[4][2] = {};
    dp[0][1] = 1;
    for (int bit = 59; bit >= 0; --bit) {
        int next_dp[4][2] = {};
        int bound_bit = (n >> bit) & 1ULL;
        for (int state = 0; state < 4; ++state) {
            for (int tight = 0; tight < 2; ++tight) {
                int limit = tight ? bound_bit : 1;
                for (int digit = 0; digit <= limit; ++digit) {
                    int next = next_state[state][digit];
                    if (next == -1) continue;
                    int next_tight = tight && digit == bound_bit;
                    int &value = next_dp[next][next_tight];
                    value += dp[state][tight];
                    if (value >= MOD) value -= MOD;
                }
            }
        }
        memcpy(dp, next_dp, sizeof(dp));
    }

    int answer = 0;
    for (int state : {2, 3}) {
        for (int tight = 0; tight < 2; ++tight) {
            answer += dp[state][tight];
            if (answer >= MOD) answer -= MOD;
        }
    }
    return answer;
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
