#include <bits/stdc++.h>
using namespace std;

using i64 = long long;
using i128 = __int128_t;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        i64 n, d;
        int m;
        cin >> n >> m >> d;

        vector<pair<i64, i64>> events(m);
        vector<i64> candidate{1, n};
        for (auto &[p, r] : events) {
            cin >> p >> r;
            candidate.push_back(p);
            if (p > 1) candidate.push_back(p - 1);
        }
        sort(candidate.begin(), candidate.end());
        candidate.erase(unique(candidate.begin(), candidate.end()), candidate.end());

        vector<i64> positions(m);
        vector<i64> pref(m + 1, 0);
        for (int i = 0; i < m; ++i) {
            positions[i] = events[i].first;
            pref[i + 1] = pref[i] + events[i].second;
        }
        const i64 cycleReward = pref[m];

        auto reward = [&](i64 length) -> i128 {
            i64 cycles = length / n;
            i64 rem = length % n;
            int count = upper_bound(positions.begin(), positions.end(), rem) - positions.begin();
            return (i128)cycles * cycleReward + pref[count];
        };

        vector<i128> candidateReward;
        candidateReward.reserve(candidate.size());
        for (i64 length : candidate) candidateReward.push_back(reward(length));

        bool possible = false;
        for (int i = 0; i < (int)candidate.size(); ++i) {
            for (int j = 0; j < (int)candidate.size(); ++j) {
                // Two runs of x and y ones separated by one zero have
                // length x+y+1.  Their reward beats the uninterrupted array
                // exactly when this inequality holds.
                if (candidateReward[i] + candidateReward[j] >
                    reward(candidate[i] + candidate[j] + 1)) {
                    possible = true;
                    break;
                }
            }
            if (possible) break;
        }
        cout << (possible ? "YES\n" : "NO\n");
    }
    return 0;
}
