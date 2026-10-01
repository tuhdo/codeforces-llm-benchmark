#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;

    while (t--) {
        long long n, d;
        int m;
        cin >> n >> m >> d;

        vector<long long> p(m), r(m), pref(m + 1, 0);
        for (int i = 0; i < m; ++i) {
            cin >> p[i] >> r[i];
            pref[i + 1] = pref[i] + r[i];
        }

        const long long total_reward = pref[m];

        auto prefix_reward = [&](long long x) -> long long {
            if (x <= 0) return 0;
            int pos = upper_bound(p.begin(), p.end(), x) - p.begin();
            return pref[pos];
        };

        auto run_reward = [&](long long length) -> long long {
            long long cycles = length / n;
            long long remainder = length % n;
            return cycles * total_reward + prefix_reward(remainder);
        };

        vector<long long> event_positions;
        event_positions.reserve(2 * m);
        for (long long x : p) {
            event_positions.push_back(x);
            event_positions.push_back(n + x);
        }

        // For a fixed a, the expression changes only at these b values.
        vector<long long> fixed_a;
        fixed_a.reserve(m + 2);
        fixed_a.push_back(1);
        fixed_a.push_back(n);
        for (long long x : p) fixed_a.push_back(x);

        bool possible = false;
        for (long long a : fixed_a) {
            vector<long long> candidates;
            candidates.reserve(3 * m + 2);
            candidates.push_back(1);
            candidates.push_back(n);
            for (long long x : p) candidates.push_back(x);

            for (long long event : event_positions) {
                // event is reached when a + b + 1 == event.
                // The value just before that downward jump is b = event-a-2.
                candidates.push_back(event - a - 2);
            }

            for (long long b : candidates) {
                if (b < 1 || b > n) continue;
                long long gain = prefix_reward(a) + prefix_reward(b)
                    - run_reward(a + b + 1) - d;
                if (gain > 0) {
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
