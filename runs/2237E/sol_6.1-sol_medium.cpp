#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<int> a(n + 1), b(n + 1), cycle_id(n + 1, -1);
        for (int i = 1; i <= n; ++i) cin >> a[i];
        for (int i = 1; i <= n; ++i) cin >> b[i];

        vector<vector<int>> cycles;
        for (int i = 1; i <= n; ++i) {
            if (cycle_id[i] != -1) continue;
            int id = (int)cycles.size();
            vector<int> cycle;
            int x = i;
            do {
                cycle_id[x] = id;
                cycle.push_back(x);
                x = a[x];
            } while (x != i);
            cycles.push_back(move(cycle));
        }

        int m = (int)cycles.size();
        vector<bool> forced(m, false), used(m, false);
        bool possible = true;

        for (int id = 0; id < m && possible; ++id) {
            int anchor = -1;
            for (int x : cycles[id]) {
                if (b[x] != -1) {
                    anchor = x;
                    break;
                }
            }
            if (anchor == -1) continue;

            int target = cycle_id[b[anchor]];
            if (cycles[id].size() != cycles[target].size() || used[target]) {
                possible = false;
                break;
            }
            forced[id] = true;
            used[target] = true;

            int x = anchor, y = b[anchor];
            for (int k = 0; k < (int)cycles[id].size(); ++k) {
                if (b[x] != -1 && b[x] != y) {
                    possible = false;
                    break;
                }
                b[x] = y;
                x = a[x];
                y = a[y];
            }
        }

        if (!possible) {
            cout << "NO\n";
            continue;
        }

        vector<vector<int>> available(n + 1);
        vector<int> next(n + 1, 0);
        for (int id = 0; id < m; ++id) {
            if (!used[id]) available[cycles[id].size()].push_back(id);
        }

        for (int id = 0; id < m; ++id) {
            if (forced[id]) continue;
            int length = (int)cycles[id].size();
            int target = available[length][next[length]++];
            for (int k = 0; k < length; ++k) {
                b[cycles[id][k]] = cycles[target][k];
            }
        }

        cout << "YES\n";
        for (int i = 1; i <= n; ++i) {
            cout << b[i] << (i == n ? '\n' : ' ');
        }
    }
}
