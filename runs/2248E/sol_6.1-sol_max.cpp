#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

struct RewardPoint {
    int64 position;
    int64 bonus;
};

bool canImprove(int64 d, const vector<RewardPoint>& candidates,
                const vector<RewardPoint>& events) {
    for (size_t i = 0; i < candidates.size(); ++i) {
        size_t nextEvent = 0;
        int64 baselineBonus = 0;
        for (size_t j = i; j < candidates.size(); ++j) {
            int64 length = candidates[i].position + candidates[j].position + 1;
            while (nextEvent < events.size() && events[nextEvent].position <= length) {
                baselineBonus = events[nextEvent].bonus;
                ++nextEvent;
            }
            if (candidates[i].bonus + candidates[j].bonus > d + baselineBonus) {
                return true;
            }
        }
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int64 n, d;
        int m;
        cin >> n >> m >> d;

        vector<RewardPoint> candidates{{0, 0}};
        vector<RewardPoint> events;
        events.reserve(2 * m);
        int64 totalBonus = 0;
        for (int i = 0; i < m; ++i) {
            int64 p, r;
            cin >> p >> r;
            totalBonus += r;
            events.push_back({p, totalBonus});
            if (p < n) {
                candidates.push_back({p, totalBonus});
            }
        }
        for (int i = 0; i < m; ++i) {
            events.push_back({n + events[i].position, totalBonus + events[i].bonus});
        }

        cout << (canImprove(d, candidates, events) ? "YES" : "NO") << '\n';
    }
    return 0;
}
