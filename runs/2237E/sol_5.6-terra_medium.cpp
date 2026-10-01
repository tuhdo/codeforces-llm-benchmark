#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        vector<int> a(n + 1), given(n + 1);
        for (int i = 1; i <= n; ++i) cin >> a[i];
        for (int i = 1; i <= n; ++i) cin >> given[i];

        vector<vector<int>> cycles;
        vector<int> cycleId(n + 1), position(n + 1);
        vector<char> seen(n + 1);
        for (int start = 1; start <= n; ++start) {
            if (seen[start]) continue;
            int id = (int)cycles.size();
            cycles.push_back({});
            int x = start;
            do {
                seen[x] = true;
                cycleId[x] = id;
                position[x] = (int)cycles.back().size();
                cycles.back().push_back(x);
                x = a[x];
            } while (x != start);
        }

        int m = (int)cycles.size();
        vector<int> target(m, -1), shift(m, -1), owner(m, -1);
        bool ok = true;
        for (int i = 1; i <= n; ++i) {
            if (given[i] == -1) continue;
            int s = cycleId[i], d = cycleId[given[i]];
            int len = (int)cycles[s].size();
            if ((int)cycles[d].size() != len) {
                ok = false;
                continue;
            }
            int offset = (position[given[i]] - position[i] + len) % len;
            if (target[s] != -1 && (target[s] != d || shift[s] != offset)) ok = false;
            if (owner[d] != -1 && owner[d] != s) ok = false;
            target[s] = d;
            shift[s] = offset;
            owner[d] = s;
        }

        vector<int> ans(n + 1, -1);
        if (ok) {
            for (int s = 0; s < m; ++s) {
                if (target[s] == -1) continue;
                int len = (int)cycles[s].size();
                for (int k = 0; k < len; ++k)
                    ans[cycles[s][k]] = cycles[target[s]][(k + shift[s]) % len];
            }
            for (int i = 1; i <= n; ++i)
                if (given[i] != -1 && ans[i] != given[i]) ok = false;
        }

        vector<set<pair<int, int>>> available(n + 1);
        if (ok) {
            for (int d = 0; d < m; ++d) {
                if (owner[d] != -1) continue;
                int smallest = *min_element(cycles[d].begin(), cycles[d].end());
                available[cycles[d].size()].insert({smallest, d});
            }

            // At the first array position from a wholly free source cycle, choose
            // the smallest value that any still unused target cycle can provide.
            for (int i = 1; i <= n; ++i) {
                if (ans[i] != -1) continue;
                int s = cycleId[i];
                int len = (int)cycles[s].size();
                if (available[len].empty()) {
                    ok = false;
                    break;
                }
                auto [value, d] = *available[len].begin();
                available[len].erase(available[len].begin());
                target[s] = d;
                owner[d] = s;
                int offset = (position[value] - position[i] + len) % len;
                for (int k = 0; k < len; ++k)
                    ans[cycles[s][k]] = cycles[d][(k + offset) % len];
            }
        }

        if (!ok) {
            cout << "NO\n";
        } else {
            cout << "YES\n";
            for (int i = 1; i <= n; ++i) cout << ans[i] << " \n"[i == n];
        }
    }
}
