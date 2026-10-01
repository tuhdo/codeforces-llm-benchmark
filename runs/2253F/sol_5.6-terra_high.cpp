#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
constexpr int64 INF = (1LL << 62);

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    vector<int64> removeCost(n + 1);
    for (int i = 1; i <= n; ++i) cin >> removeCost[i];

    int64 answer = 0;

    // Every number has a unique form root * 2^a * 3^b, where gcd(root, 6)=1.
    // Edges {x, 2x, 3x, 4x} never connect two different roots.
    for (int root = 1; 4LL * root <= n; ++root) {
        if (root % 2 == 0 || root % 3 == 0) continue;

        int64 rowBase = root;
        int rowLength = 0;
        for (int64 value = rowBase; value <= n; value *= 2) ++rowLength;

        const int initialStates = 1 << rowLength;
        vector<int64> previous(initialStates, 0);
        vector<int64> subsetCost(initialStates, 0);
        for (int mask = 1; mask < initialStates; ++mask) {
            int bit = __builtin_ctz(mask);
            subsetCost[mask] = subsetCost[mask ^ (1 << bit)] + removeCost[rowBase << bit];
        }
        previous.swap(subsetCost);
        int stateCount = initialStates;

        // A mask says which elements in the current row are removed.  A run of
        // three kept entries at a,a+1,a+2 forces removal of a in the next row.
        while (rowLength >= 3) {
            const int64 nextBase = rowBase * 3;
            int nextLength = 0;
            for (int64 value = nextBase; value <= n; value *= 2) ++nextLength;

            const int nextStates = 1 << nextLength;
            vector<int64> bestForced(nextStates, INF);

            const unsigned int allCurrent = (1u << rowLength) - 1;
            const unsigned int possibleStarts = (1u << (rowLength - 2)) - 1;
            for (int mask = 0; mask < stateCount; ++mask) {
                unsigned int kept = (~static_cast<unsigned int>(mask)) & allCurrent;
                unsigned int forced = kept & (kept >> 1) & (kept >> 2) & possibleStarts;
                bestForced[forced] = min(bestForced[forced], previous[mask]);
            }

            // For each next-row mask, find the cheapest preceding row whose
            // required removals are a subset of that mask.
            for (int bit = 0; bit < nextLength; ++bit) {
                const int step = 1 << bit;
                for (int mask = 0; mask < nextStates; ++mask) {
                    if (mask & step) {
                        bestForced[mask] = min(bestForced[mask], bestForced[mask ^ step]);
                    }
                }
            }

            vector<int64> current(nextStates, 0);
            vector<int64> currentCost(nextStates, 0);
            for (int mask = 1; mask < nextStates; ++mask) {
                int bit = __builtin_ctz(mask);
                currentCost[mask] = currentCost[mask ^ (1 << bit)] + removeCost[nextBase << bit];
            }
            for (int mask = 0; mask < nextStates; ++mask) {
                current[mask] = bestForced[mask] + currentCost[mask];
            }

            previous.swap(current);
            stateCount = nextStates;
            rowBase = nextBase;
            rowLength = nextLength;
        }

        answer += *min_element(previous.begin(), previous.end());
    }

    cout << answer << '\n';
    return 0;
}
