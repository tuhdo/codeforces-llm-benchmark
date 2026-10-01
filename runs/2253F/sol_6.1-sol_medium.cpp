#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
constexpr int64 INF = numeric_limits<int64>::max() / 4;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<int64> cost(n + 1);
    for (int i = 1; i <= n; ++i) cin >> cost[i];

    vector<int64> removedCost;
    int64 answer = 0;
    for (int base = 1; base <= n / 4; ++base) {
        if (base % 2 == 0 || base % 3 == 0) continue;

        // incoming[D] is the cheapest processed prefix whose required
        // deletions in this row are a subset of D.
        vector<int64> incoming(1, 0);
        int incomingBits = 0;

        for (int64 rowBase = base; ; rowBase *= 3) {
            vector<int64> weights;
            int64 total = 0;
            for (int64 value = rowBase; value <= n; value *= 2) {
                weights.push_back(cost[value]);
                total += cost[value];
            }

            int width = static_cast<int>(weights.size());
            int states = 1 << width;
            int incomingMask = (1 << incomingBits) - 1;
            int outgoingBits = max(0, width - 2);
            vector<int64> next(1 << outgoingBits, INF);
            removedCost.resize(states);
            removedCost[0] = total;
            int64 minimum = INF;

            for (int kept = 0; kept < states; ++kept) {
                if (kept != 0) {
                    int bit = __builtin_ctz(static_cast<unsigned>(kept));
                    removedCost[kept] = removedCost[kept & (kept - 1)] - weights[bit];
                }
                int deleted = incomingMask & ~kept;
                int64 candidate = incoming[deleted] + removedCost[kept];
                minimum = min(minimum, candidate);

                // Three consecutive kept powers of 2 force the corresponding
                // number in the next power-of-3 row to be deleted.
                int required = kept & (kept >> 1) & (kept >> 2);
                next[required] = min(next[required], candidate);
            }

            if (width <= 2) {
                answer += minimum;
                break;
            }

            // Subset-minimum transform: next[D] = min over R subset of D.
            for (int bit = 0; bit < outgoingBits; ++bit) {
                int half = 1 << bit;
                for (int start = 0; start < static_cast<int>(next.size()); start += 2 * half) {
                    for (int offset = 0; offset < half; ++offset) {
                        next[start + half + offset] = min(next[start + half + offset], next[start + offset]);
                    }
                }
            }
            incoming = move(next);
            incomingBits = outgoingBits;
        }
    }

    cout << answer << '\n';
    return 0;
}
