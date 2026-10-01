#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

using int64 = long long;
using i128 = __int128_t;

struct Segment {
    int64 left;
    int64 right;
    i128 offset;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCount;
    cin >> testCount;

    while (testCount--) {
        int64 n, d;
        int m;
        cin >> n >> m >> d;

        vector<pair<int64, int64>> rewards(m);
        int64 totalReward = 0;
        vector<int64> prefixRewards(m + 1, 0);
        for (auto &[p, r] : rewards) {
            cin >> p >> r;
            totalReward += r;
        }
        for (int i = 0; i < m; ++i) {
            prefixRewards[i + 1] = prefixRewards[i] + rewards[i].second;
        }

        if (n == 1) {
            cout << "NO\n";
            continue;
        }

        // For a run of k < n ones, use item length k + 1 (the run plus
        // the zero that separates it from the next run).
        vector<int64> boundaries{2, n + 1};
        for (const auto &[p, r] : rewards) {
            if (p < n) boundaries.push_back(p + 1);
        }
        sort(boundaries.begin(), boundaries.end());
        boundaries.erase(unique(boundaries.begin(), boundaries.end()), boundaries.end());

        vector<Segment> segments;
        int rewardIndex = 0;
        int64 prefixBefore = 0;
        for (size_t i = 0; i + 1 < boundaries.size(); ++i) {
            int64 left = boundaries[i];
            int64 right = boundaries[i + 1] - 1;
            while (rewardIndex < m && rewards[rewardIndex].first < left) {
                prefixBefore += rewards[rewardIndex].second;
                ++rewardIndex;
            }
            if (left <= right && left <= n) {
                i128 offset = (i128)n * d - (i128)n * prefixBefore;
                segments.push_back({left, min(right, n), offset});
            }
        }

        auto prefixRewardBefore = [&](int64 position) -> int64 {
            // Sum r_i for p_i < position, which is the prefix through position - 1.
            auto it = lower_bound(
                rewards.begin(), rewards.end(), position,
                [](const auto &entry, int64 value) { return entry.first < value; });
            return prefixRewards[it - rewards.begin()];
        };

        bool possible = false;
        for (size_t i = 0; i < segments.size() && !possible; ++i) {
            for (size_t j = i; j < segments.size() && !possible; ++j) {
                const auto &a = segments[i];
                const auto &b = segments[j];
                int64 minSum = a.left + b.left;
                int64 maxSum = a.right + b.right;

                for (int wrap = 0; wrap <= 1; ++wrap) {
                    int64 cLeft = max<int64>(1, minSum - wrap * n);
                    int64 cRight = min<int64>(n, maxSum - wrap * n);
                    if (cLeft > cRight) continue;

                    // V_c = n*d - n*prefixReward(c - 1) is nonincreasing in c,
                    // so cLeft gives the smallest possible pair difference.
                    i128 offsetC = (i128)n * d - (i128)n * prefixRewardBefore(cLeft);
                    i128 difference = (i128)wrap * n * totalReward
                                    + a.offset + b.offset - offsetC;
                    if (difference < 0) {
                        possible = true;
                        break;
                    }
                }
            }
        }

        cout << (possible ? "YES\n" : "NO\n");
    }

    return 0;
}
