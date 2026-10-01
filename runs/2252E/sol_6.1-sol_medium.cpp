#include <array>
#include <iostream>

using namespace std;

constexpr int MOD = 1'000'000'007;

int solve(unsigned long long n) {
    // States: 0 = before the first block, 1 = a block needs its final 1,
    //         2 = after a complete block.
    array<array<int, 2>, 3> dp{};
    dp[0][1] = 1;

    for (int bit = 59; bit >= 0; --bit) {
        array<array<int, 2>, 3> next{};
        int bound = (n >> bit) & 1ULL;

        for (int state = 0; state < 3; ++state) {
            for (int tight = 0; tight < 2; ++tight) {
                int ways = dp[state][tight];
                if (ways == 0) continue;

                auto add = [&](int digit, int next_state) {
                    if (tight && digit > bound) return;
                    int next_tight = tight && digit == bound;
                    int& value = next[next_state][next_tight];
                    value += ways;
                    if (value >= MOD) value -= MOD;
                };

                if (state == 0) {
                    add(0, 0);
                    add(1, 1);
                } else if (state == 1) {
                    add(1, 2);
                } else {
                    add(0, 2);  // A single zero.
                    add(0, 1);  // Start a 01 block.
                    add(1, 1);  // Start a 11 block.
                }
            }
        }
        dp = next;
    }

    return (dp[2][0] + dp[2][1]) % MOD;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        unsigned long long n;
        cin >> n;
        cout << solve(n) << '\n';
    }
}
