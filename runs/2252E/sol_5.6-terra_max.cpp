#include <array>
#include <iostream>

using namespace std;

constexpr long long MOD = 1'000'000'007LL;

int next_state(int state, int bit) {
    // 0: no leading 1 yet
    // 1, 2: current leading run of 1s has odd/even length
    // 3: an even leading run has ended
    // 4: an odd leading run has ended
    if (state == 0) return bit ? 1 : 0;
    if (state == 1) return bit ? 2 : 4;
    if (state == 2) return bit ? 1 : 3;
    return state;
}

long long count_valid_endpoints(long long n) {
    array<array<long long, 2>, 5> dp{};
    dp[0][1] = 1;

    // n < 2^60, so bits 59 through 0 cover every possible endpoint.
    for (int pos = 59; pos >= 0; --pos) {
        array<array<long long, 2>, 5> next{};
        const int n_bit = static_cast<int>((n >> pos) & 1LL);

        for (int state = 0; state < 5; ++state) {
            for (int tight = 0; tight <= 1; ++tight) {
                const long long ways = dp[state][tight];
                if (ways == 0) continue;

                const int limit = tight ? n_bit : 1;
                for (int bit = 0; bit <= limit; ++bit) {
                    const int new_state = next_state(state, bit);
                    const int new_tight = tight && (bit == n_bit);
                    next[new_state][new_tight] += ways;
                    if (next[new_state][new_tight] >= MOD) {
                        next[new_state][new_tight] -= MOD;
                    }
                }
            }
        }
        dp = next;
    }

    // State 2 means the whole number is an even run of 1s.
    // State 3 means its leading run ended after an even number of 1s.
    return (dp[2][0] + dp[2][1] + dp[3][0] + dp[3][1]) % MOD;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        long long n;
        cin >> n;
        cout << count_valid_endpoints(n) << '\n';
    }
}
