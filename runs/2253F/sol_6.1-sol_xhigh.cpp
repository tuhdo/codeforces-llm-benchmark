#include <algorithm>
#include <bit>
#include <iostream>
#include <limits>
#include <vector>

using namespace std;
using int64 = long long;

static vector<int64> row_costs(int base, int n, const vector<int64>& cost) {
    vector<int64> weights;
    for (int64 value = base; value <= n; value *= 2) {
        weights.push_back(cost[value]);
    }

    const int states = 1 << weights.size();
    vector<int64> result(states, 0);
    for (unsigned mask = 1; mask < static_cast<unsigned>(states); ++mask) {
        result[mask] = result[mask & (mask - 1)] + weights[countr_zero(mask)];
    }
    return result;
}

static int64 solve_component(int base, int n, const vector<int64>& cost) {
    // A mask records the removed numbers base * 2^a in the current row.
    vector<int64> dp = row_costs(base, n, cost);
    const int64 infinity = numeric_limits<int64>::max() / 4;

    while (dp.size() >= 8) {
        const int states = static_cast<int>(dp.size());
        const int forced_states = states / 4;
        const unsigned forced_mask = forced_states - 1;
        vector<int64> best(forced_states, infinity);

        for (unsigned mask = 0; mask < static_cast<unsigned>(states); ++mask) {
            // Keeping three consecutive powers of two forces removal of 3k.
            const unsigned forced = forced_mask & ~(mask | (mask >> 1) | (mask >> 2));
            best[forced] = min(best[forced], dp[mask]);
        }

        // best[S] becomes the minimum over all forced masks contained in S.
        for (int bit = 1; bit < forced_states; bit <<= 1) {
            for (int block = 0; block < forced_states; block += 2 * bit) {
                for (int offset = 0; offset < bit; ++offset) {
                    const int index = block + bit + offset;
                    best[index] = min(best[index], best[index - bit]);
                }
            }
        }

        base *= 3;
        vector<int64> next = row_costs(base, n, cost);
        for (unsigned mask = 0; mask < next.size(); ++mask) {
            next[mask] += best[mask & forced_mask];
        }
        dp = move(next);
    }

    return *min_element(dp.begin(), dp.end());
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<int64> cost(n + 1);
    for (int i = 1; i <= n; ++i) {
        cin >> cost[i];
    }

    // The only progressions of characteristic 4 are {k, 2k, 3k, 4k}.
    int64 answer = 0;
    for (int base = 1; base <= n / 4; ++base) {
        if (base % 2 != 0 && base % 3 != 0) {
            answer += solve_component(base, n, cost);
        }
    }

    cout << answer << '\n';
    return 0;
}
