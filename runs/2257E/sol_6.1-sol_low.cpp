#include <bits/stdc++.h>
using namespace std;

using ll = long long;

struct Segment {
    ll need, gain;
    int end;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        ll capital;
        cin >> n >> capital;
        vector<vector<ll>> a(n), b(n);
        vector<vector<Segment>> segments(n);
        vector<int> next(n, 0), built(n, 0);
        using Entry = pair<ll, int>;
        priority_queue<Entry, vector<Entry>, greater<Entry>> pq;

        for (int i = 0; i < n; ++i) {
            int m;
            cin >> m;
            a[i].resize(m);
            b[i].resize(m);
            for (ll &v : a[i]) cin >> v;
            for (ll &v : b[i]) cin >> v;
            ll gain = 0, need = 0;
            for (int j = 0; j < m; ++j) {
                need = max(need, a[i][j] - gain);
                gain += b[i][j] - a[i][j];
                if (gain >= 0) {
                    segments[i].push_back({need, gain, j + 1});
                    gain = need = 0;
                }
            }
            if (!segments[i].empty()) {
                pq.emplace(segments[i][0].need, i);
            }
        }

        while (!pq.empty() && pq.top().first <= capital) {
            int i = pq.top().second;
            pq.pop();
            const Segment &s = segments[i][next[i]++];
            capital += s.gain;
            built[i] = s.end;
            if (next[i] < (int)segments[i].size()) {
                pq.emplace(segments[i][next[i]].need, i);
            }
        }

        int bestHeight = -1, bestIndex = -1;
        for (int i = 0; i < n; ++i) {
            ll money = capital;
            int height = built[i];
            while (height < (int)a[i].size() && money >= a[i][height]) {
                money += b[i][height] - a[i][height];
                ++height;
            }
            if (height > bestHeight) {
                bestHeight = height;
                bestIndex = i + 1;
            }
        }
        cout << bestHeight << ' ' << bestIndex << '\n';
    }
}
