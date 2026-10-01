#include <bits/stdc++.h>
using namespace std;

struct Floor {
    long long cost, gain;
    int building, pos;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        long long money;
        cin >> n >> money;
        vector<vector<Floor>> project(n);
        for (int i = 0; i < n; ++i) {
            int m;
            cin >> m;
            vector<long long> a(m), b(m);
            for (auto &v : a) cin >> v;
            for (auto &v : b) cin >> v;
            project[i].reserve(m);
            for (int j = 0; j < m; ++j)
                project[i].push_back({a[j], b[j], i, j});
        }

        // Among currently executable contracts, doing the one with the
        // largest balance change leaves at least as much capital as any
        // other immediate choice.  A heap plus a cost-ordered waiting set
        // maintains the currently executable frontier of every chain.
        struct ByCost {
            bool operator()(const Floor* x, const Floor* y) const {
                if (x->cost != y->cost) return x->cost < y->cost;
                return x->building < y->building;
            }
        };
        struct ByDelta {
            bool operator()(const Floor* x, const Floor* y) const {
                long long dx = x->gain - x->cost, dy = y->gain - y->cost;
                if (dx != dy) return dx < dy;
                if (x->building != y->building) return x->building > y->building;
                return x->pos > y->pos;
            }
        };
        set<Floor*, ByCost> waiting;
        priority_queue<Floor*, vector<Floor*>, ByDelta> ready;
        vector<int> at(n, 0), done(n, 0);
        for (int i = 0; i < n; ++i) waiting.insert(&project[i][0]);

        while (true) {
            while (!waiting.empty() && (*waiting.begin())->cost <= money) {
                Floor *f = *waiting.begin();
                waiting.erase(waiting.begin());
                ready.push(f);
            }
            if (ready.empty()) break;
            Floor *f = ready.top();
            ready.pop();
            if (f->cost > money) {
                waiting.insert(f);
                continue;
            }
            money += f->gain - f->cost;
            int i = f->building;
            ++done[i];
            ++at[i];
            if (at[i] < (int)project[i].size()) waiting.insert(&project[i][at[i]]);
        }

        int best = 0, id = 0;
        for (int i = 0; i < n; ++i) {
            if (done[i] > best) best = done[i], id = i;
        }
        cout << best << ' ' << id + 1 << '\n';
    }
}
