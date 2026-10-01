#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCount;
    cin >> testCount;

    while (testCount--) {
        long long n, d;
        int m;
        cin >> n >> m >> d;

        vector<long long> p(m), prefix(m + 1, 0);
        for (int i = 0; i < m; ++i) {
            long long reward;
            cin >> p[i] >> reward;
            prefix[i + 1] = prefix[i] + reward;
        }

        const long long totalReward = prefix[m];

        // A useful run length is either zero or a position at which a reward
        // is received. Positions equal to n reduce to a zero-length residue.
        vector<pair<long long, long long>> runs;
        runs.emplace_back(0, 0);
        for (int i = 0; i < m; ++i) {
            if (p[i] < n) {
                runs.emplace_back(p[i], prefix[i + 1]);
            }
        }

        auto rewardsUpTo = [&](long long length) -> long long {
            int count = upper_bound(p.begin(), p.end(), length) - p.begin();
            return prefix[count];
        };

        bool possible = false;
        for (const auto& [firstLength, firstReward] : runs) {
            for (const auto& [secondLength, secondReward] : runs) {
                const long long length = firstLength + secondLength + 1;
                const long long baselineRewards =
                    length < n ? rewardsUpTo(length)
                               : totalReward + rewardsUpTo(length - n);

                if (firstReward + secondReward > d + baselineRewards) {
                    possible = true;
                    break;
                }
            }
            if (possible) {
                break;
            }
        }

        cout << (possible ? "YES\n" : "NO\n");
    }
}
