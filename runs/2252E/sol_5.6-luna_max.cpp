#include <bits/stdc++.h>

using namespace std;

constexpr long long MOD = 1'000'000'007LL;

// Number of choices for a fixed highest bit H when there is no upper bound
// on c.  The highest bit of k is H, and every lower set bit of k may or may
// not contribute its shifted bit to c.
array<long long, 64> unrestricted_ways() {
    array<long long, 64> ways{};
    ways[0] = ways[1] = 1;
    for (int h = 2; h < static_cast<int>(ways.size()); ++h) {
        ways[h] = (ways[h - 1] + 2 * ways[h - 2]) % MOD;
    }
    return ways;
}

long long count_for_highest_bit(unsigned long long n, int h,
                                const array<long long, 64>& ways) {
    using u128 = unsigned __int128;

    // The smallest c for this h is 3 * 2^h.
    if ((u128(3) << h) > n) {
        return 0;
    }

    // Every c with highest bit h + 1 is below 2^(h + 2).
    if (u128(n) >= (u128(1) << (h + 2))) {
        return ways[h];
    }

    // Here n has the same highest bit h + 1 as c, so compare the remaining
    // bits with a most-significant-bit-first digit DP.
    if (((n >> h) & 1ULL) == 0) {
        return 0;
    }

    // dp[previous_k_bit][less] after the already compared prefix.
    long long dp[2][2]{};
    dp[1][0] = 1;  // k_h = 1; c_h = 1 as k_{h-1} must be zero.

    for (int i = h - 1; i >= 0; --i) {
        long long next[2][2]{};

        for (int previous = 0; previous <= 1; ++previous) {
            for (int less = 0; less <= 1; ++less) {
                const long long current_count = dp[previous][less];
                if (current_count == 0) {
                    continue;
                }

                for (int current = 0; current <= 1; ++current) {
                    if (previous && current) {
                        continue;  // k has no adjacent set bits.
                    }

                    const int optional_count = current ? 2 : 1;
                    for (int optional = 0; optional < optional_count;
                         ++optional) {
                        // c_{i+1} = k_{i+1} OR optional_{i}.
                        const int c_bit = previous | optional;
                        const int n_bit = static_cast<int>((n >> (i + 1)) & 1ULL);

                        if (!less && c_bit > n_bit) {
                            continue;
                        }

                        const int next_less = less || (c_bit < n_bit);
                        long long& cell = next[current][next_less];
                        cell += current_count;
                        if (cell >= MOD) {
                            cell -= MOD;
                        }
                    }
                }
            }
        }

        memcpy(dp, next, sizeof(dp));
    }

    // The last remaining bit is c_0 = k_0.
    long long answer = 0;
    const int n_bit = static_cast<int>(n & 1ULL);
    for (int current = 0; current <= 1; ++current) {
        for (int less = 0; less <= 1; ++less) {
            if (!less && current > n_bit) {
                continue;
            }
            answer += dp[current][less];
        }
    }
    return answer % MOD;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const auto ways = unrestricted_ways();

    int t;
    cin >> t;
    while (t--) {
        unsigned long long n;
        cin >> n;

        long long answer = 0;
        for (int h = 0; h < 63; ++h) {
            answer += count_for_highest_bit(n, h, ways);
            if (answer >= MOD) {
                answer -= MOD;
            }
        }
        cout << answer << '\n';
    }
    return 0;
}
