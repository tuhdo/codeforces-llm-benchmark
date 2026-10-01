#include <array>
#include <iostream>

using namespace std;

constexpr int MOD = 1'000'000'007;

void add(int& destination, int value) {
    destination += value;
    if (destination >= MOD) destination -= MOD;
}

int count_triplets(unsigned long long n) {
    // States: leading zeros, between blocks, awaiting the final 1 of a block.
    // After the initial block 11, allowed blocks are 0, 01, and 11.
    array<array<int, 2>, 3> dp{};
    dp[0][1] = 1;

    for (int bit = 59; bit >= 0; --bit) {
        array<array<int, 2>, 3> next{};
        int bound_bit = (n >> bit) & 1ULL;

        for (int state = 0; state < 3; ++state) {
            for (int tight = 0; tight < 2; ++tight) {
                int ways = dp[state][tight];
                if (ways == 0) continue;

                int limit = tight ? bound_bit : 1;
                for (int digit = 0; digit <= limit; ++digit) {
                    int next_tight = tight && digit == bound_bit;
                    if (state == 0) {
                        add(next[digit == 0 ? 0 : 2][next_tight], ways);
                    } else if (state == 1) {
                        if (digit == 0) add(next[1][next_tight], ways);
                        add(next[2][next_tight], ways);
                    } else if (digit == 1) {
                        add(next[1][next_tight], ways);
                    }
                }
            }
        }
        dp = next;
    }

    int answer = dp[1][0];
    add(answer, dp[1][1]);
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
}
