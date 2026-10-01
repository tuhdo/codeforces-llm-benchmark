#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

struct Reward {
    int64 position;
    int64 value;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int64 n, d;
        int m;
        cin >> n >> m >> d;

        vector<Reward> rewards(m);
        vector<int64> positions{0}, prefix{0};
        int64 sum = 0;
        for (auto& reward : rewards) {
            cin >> reward.position >> reward.value;
            sum += reward.value;
            if (reward.position < n) {
                positions.push_back(reward.position);
                prefix.push_back(sum);
            }
        }

        vector<Reward> events;
        events.reserve(2 * m);
        for (int cycle = 0; cycle < 2; ++cycle) {
            for (const auto& reward : rewards) {
                events.push_back({reward.position + cycle * n, reward.value});
            }
        }

        bool possible = false;
        for (size_t i = 0; i < positions.size() && !possible; ++i) {
            size_t next = 0;
            int64 baseline = 0;
            for (size_t j = 0; j < positions.size(); ++j) {
                int64 length = positions[i] + positions[j] + 1;
                while (next < events.size() && events[next].position <= length) {
                    baseline += events[next].value;
                    ++next;
                }
                if (prefix[i] + prefix[j] > baseline + d) {
                    possible = true;
                    break;
                }
            }
        }

        cout << (possible ? "YES\n" : "NO\n");
    }
}
