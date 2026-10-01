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

        vector<pair<int64, int64>> events;
        vector<int64> lengths{0}, rewards{0};
        int64 prefix = 0;
        for (int i = 0; i < m; ++i) {
            int64 p, r;
            cin >> p >> r;
            events.emplace_back(p, r);
            prefix += r;
            if (p < n) {
                lengths.push_back(p);
                rewards.push_back(prefix);
            }
        }
        // A pair of reduced runs needs at most two reward cycles.
        for (int i = 0; i < m; ++i) {
            events.emplace_back(events[i].first + n, events[i].second);
        }

        bool possible = false;
        for (size_t i = 0; i < lengths.size() && !possible; ++i) {
            size_t next = 0;
            int64 all_ones_reward = 0;
            for (size_t j = 0; j < lengths.size(); ++j) {
                int64 total_length = lengths[i] + lengths[j] + 1;
                while (next < events.size() && events[next].first <= total_length) {
                    all_ones_reward += events[next].second;
                    ++next;
                }
                if (rewards[i] + rewards[j] > all_ones_reward + d) {
                    possible = true;
                    break;
                }
            }
        }
        cout << (possible ? "YES" : "NO") << '\n';
    }
}
