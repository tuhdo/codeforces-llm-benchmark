#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    vector<int64> cost(n + 1);
    for (int i = 1; i <= n; ++i) cin >> cost[i];

    const int64 NEG = -(1LL << 60);
    int64 answer = 0;

    // A component contains exactly the numbers core * 2^a * 3^b,
    // where core is not divisible by 2 or 3.
    for (int core = 1; core * 4 <= n; ++core) {
        if (core % 2 == 0 || core % 3 == 0) continue;

        auto maskWeights = [&](int factor) {
            vector<int64> values;
            for (int value = factor; value <= n; value *= 2) {
                values.push_back(cost[value]);
                if (value > n / 2) break;
            }

            const int size = 1 << static_cast<int>(values.size());
            vector<int64> result(size);
            for (int mask = 1; mask < size; ++mask) {
                const int bit = __builtin_ctz(static_cast<unsigned>(mask));
                result[mask] = result[mask ^ (1 << bit)] + values[bit];
            }
            return result;
        };

        int factor = core;
        vector<int64> dp = maskWeights(factor);
        int64 componentTotal = dp.back();

        while (factor <= n / 3) {
            const int nextFactor = factor * 3;
            vector<int64> nextWeight = maskWeights(nextFactor);
            componentTotal += nextWeight.back();

            const int currentLength = __lg(static_cast<unsigned>(dp.size()));
            const int startCount = currentLength - 2;
            vector<int64> nextDp(nextWeight.size(), NEG);

            if (startCount <= 0) {
                const int64 best = *max_element(dp.begin(), dp.end());
                for (int mask = 0; mask < static_cast<int>(nextDp.size()); ++mask) {
                    nextDp[mask] = best + nextWeight[mask];
                }
            } else {
                const int startMaskCount = 1 << startCount;
                vector<int64> bestByTriple(startMaskCount, NEG);

                for (int mask = 0; mask < static_cast<int>(dp.size()); ++mask) {
                    const int triples = mask & (mask >> 1) & (mask >> 2);
                    bestByTriple[triples] = max(bestByTriple[triples], dp[mask]);
                }

                // bestByTriple[mask] becomes the best previous row whose
                // completed-triple mask is a submask of mask.
                for (int bit = 0; bit < startCount; ++bit) {
                    for (int mask = 0; mask < startMaskCount; ++mask) {
                        if (mask & (1 << bit)) {
                            bestByTriple[mask] = max(
                                bestByTriple[mask],
                                bestByTriple[mask ^ (1 << bit)]
                            );
                        }
                    }
                }

                const int relevantBits = startMaskCount - 1;
                for (int mask = 0; mask < static_cast<int>(nextDp.size()); ++mask) {
                    const int allowed = relevantBits ^ (mask & relevantBits);
                    nextDp[mask] = bestByTriple[allowed] + nextWeight[mask];
                }
            }

            dp.swap(nextDp);
            factor = nextFactor;
        }

        const int64 bestKept = *max_element(dp.begin(), dp.end());
        answer += componentTotal - bestKept;
    }

    cout << answer << '\n';
    return 0;
}
