#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

using int64 = long long;

static int64 solveOneDimensional(const vector<int64>& a, int k) {
    const int len = static_cast<int>(a.size());

    if (len == 1) {
        return a[0] >= 0 ? 0 : -1;
    }
    if (len == 2) {
        return k == 1 ? 0 : llabs(a[0] - a[1]);
    }

    int64 total = 0;
    for (int64 value : a) {
        total += value;
    }

    vector<int64> score(len);
    for (int i = 0; i < len; ++i) {
        score[i] = 2 * a[i] - total;
    }

    vector<int64> inner(score.begin() + 1, score.end() - 1);
    sort(inner.rbegin(), inner.rend());

    const int64 fullGain = len - 2;

    auto feasible = [&](int64 operations) {
        const int64 leftBase = score[0] + operations * fullGain;
        const int64 rightBase = score[len - 1] + operations * fullGain;

        // We ask for e endpoint peaks and obtain the remaining k-e from the
        // inner cells.  At least e is enough: extra endpoint peaks are harmless.
        for (int endpointNeed = 0; endpointNeed <= 2; ++endpointNeed) {
            const int innerNeed = max(0, k - endpointNeed);
            if (innerNeed > static_cast<int>(inner.size())) {
                continue;
            }

            int64 upperT = operations;
            if (innerNeed > 0) {
                upperT = min(upperT, inner[innerNeed - 1] + operations * fullGain);
            }
            if (upperT < 0) {
                continue;
            }

            if (endpointNeed == 0) {
                return true;
            }

            if (endpointNeed == 1) {
                // With t non-full operations, using all of them to exclude one
                // endpoint gives that endpoint an extra t score.
                const int64 needLeft = max<int64>(0, -leftBase);
                const int64 needRight = max<int64>(0, -rightBase);
                if (min(needLeft, needRight) <= upperT) {
                    return true;
                }
                continue;
            }

            // Let x of the t operations exclude the left endpoint and t-x
            // exclude the right endpoint.  Checking two consecutive t values
            // handles the only remaining parity condition.
            const int64 lowerT = max({int64(0), -leftBase, -rightBase});
            for (int64 t : {lowerT, lowerT + 1}) {
                if (t > upperT) {
                    continue;
                }
                const int64 minX = (t - leftBase + 1) / 2;
                const int64 maxX = (rightBase + t) / 2;
                if (minX <= min(t, maxX)) {
                    return true;
                }
            }
        }
        return false;
    };

    int64 high = 0;
    for (int64 value : score) {
        if (value < 0) {
            high = max(high, (-value + fullGain - 1) / fullGain);
        }
    }

    int64 low = 0;
    while (low < high) {
        const int64 mid = low + (high - low) / 2;
        if (feasible(mid)) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }
    return low;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCount;
    cin >> testCount;
    while (testCount--) {
        int n, m, k;
        cin >> n >> m >> k;

        vector<vector<int64>> value(n, vector<int64>(m));
        vector<int64> rowSum(n, 0), colSum(m, 0);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                cin >> value[i][j];
                rowSum[i] += value[i][j];
                colSum[j] += value[i][j];
            }
        }

        if (n == 1 || m == 1) {
            vector<int64> line;
            if (n == 1) {
                line = value[0];
            } else {
                line.resize(n);
                for (int i = 0; i < n; ++i) {
                    line[i] = value[i][0];
                }
            }
            cout << solveOneDimensional(line, k) << '\n';
            continue;
        }

        const int64 fullGain = n + m - 3;
        vector<int64> needed;
        needed.reserve(n * m);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                const int64 score = 3 * value[i][j] - rowSum[i] - colSum[j];
                needed.push_back(score >= 0 ? 0 : (-score + fullGain - 1) / fullGain);
            }
        }
        nth_element(needed.begin(), needed.begin() + (k - 1), needed.end());
        cout << needed[k - 1] << '\n';
    }
    return 0;
}
