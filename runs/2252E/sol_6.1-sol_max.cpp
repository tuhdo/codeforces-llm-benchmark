#include <bits/stdc++.h>
using namespace std;

constexpr int MOD = 1'000'000'007;

enum State { START, ODD, EVEN, DONE };

constexpr int NEXT[4][2] = {
    {START, ODD},
    {-1, EVEN},
    {DONE, ODD},
    {DONE, DONE}
};

int count_triplets(long long n) {
    int bits = 0;
    for (long long value = n; value > 0; value >>= 1) {
        ++bits;
    }

    array<array<int, 4>, 2> dp{};
    dp[1][START] = 1;

    for (int bit = bits - 1; bit >= 0; --bit) {
        array<array<int, 4>, 2> next{};
        int bound = (n >> bit) & 1;

        for (int tight = 0; tight < 2; ++tight) {
            for (int state = 0; state < 4; ++state) {
                if (dp[tight][state] == 0) continue;

                for (int digit = 0; digit <= (tight ? bound : 1); ++digit) {
                    int next_state = NEXT[state][digit];
                    if (next_state == -1) continue;

                    int next_tight = tight && (digit == bound);
                    int& ways = next[next_tight][next_state];
                    ways += dp[tight][state];
                    if (ways >= MOD) ways -= MOD;
                }
            }
        }

        dp = next;
    }

    long long answer = 0;
    for (int tight = 0; tight < 2; ++tight) {
        answer += dp[tight][EVEN];
        answer += dp[tight][DONE];
    }
    return answer % MOD;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        long long n;
        cin >> n;
        cout << count_triplets(n) << '\n';
    }
}
