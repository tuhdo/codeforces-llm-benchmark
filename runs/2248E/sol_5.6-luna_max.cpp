#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;

    while (t--) {
        int64 n, d;
        int m;
        cin >> n >> m >> d;

        vector<int64> p(m), r(m), prefix(m + 1, 0);
        for (int i = 0; i < m; ++i) {
            cin >> p[i] >> r[i];
            prefix[i + 1] = prefix[i] + r[i];
        }

        if (n == 1) {
            cout << "NO\n";
            continue;
        }

        const int64 total_reward = prefix[m];

        // On [1, n - 1], the reward of an all-one run changes only at p_i.
        vector<int64> candidates{1};
        for (int64 position : p) {
            if (position < n) {
                candidates.push_back(position);
            }
        }
        sort(candidates.begin(), candidates.end());
        candidates.erase(unique(candidates.begin(), candidates.end()), candidates.end());

        auto run_reward = [&](int64 length) -> int64 {
            const int64 full_cycles = length / n;
            const int64 remainder = length % n;
            const auto it = upper_bound(p.begin(), p.end(), remainder);
            const int count = static_cast<int>(it - p.begin());
            return full_cycles * total_reward + prefix[count];
        };

        bool possible = false;
        for (int64 x : candidates) {
            const int64 left_reward = run_reward(x);
            for (int64 y : candidates) {
                // Compare 1^x 0 1^y with the all-one array of the same length.
                if (left_reward + run_reward(y) > d + run_reward(x + y + 1)) {
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

    return 0;
}
