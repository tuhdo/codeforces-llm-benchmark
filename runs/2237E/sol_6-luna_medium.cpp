#include <bits/stdc++.h>
using namespace std;

struct Cycle {
    vector<int> nodes;
    int len;
    int anchor;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        vector<int> a(n), b(n);
        for (int &x : a) { cin >> x; --x; }
        for (int &x : b) cin >> x;

        vector<int> cid(n, -1), pos(n, -1);
        vector<Cycle> cycles;
        for (int i = 0; i < n; ++i) if (cid[i] == -1) {
            int id = (int)cycles.size();
            Cycle c;
            int v = i;
            do {
                cid[v] = id;
                pos[v] = (int)c.nodes.size();
                c.nodes.push_back(v);
                v = a[v];
            } while (v != i);
            c.len = (int)c.nodes.size();
            c.anchor = *min_element(c.nodes.begin(), c.nodes.end());
            cycles.push_back(move(c));
        }

        int m = (int)cycles.size();
        vector<int> target(m, -1), shift(m, -1), sourceOfTarget(m, -1);
        bool ok = true;
        for (int i = 0; i < n && ok; ++i) if (b[i] != -1) {
            int v = b[i] - 1;
            int s = cid[i], d = cid[v];
            if (cycles[s].len != cycles[d].len) { ok = false; break; }
            int sh = (pos[v] - pos[i] + cycles[s].len) % cycles[s].len;
            if (target[s] != -1 && (target[s] != d || shift[s] != sh)) { ok = false; break; }
            target[s] = d;
            shift[s] = sh;
        }
        if (ok) {
            for (int s = 0; s < m; ++s) if (target[s] != -1) {
                int d = target[s];
                if (sourceOfTarget[d] != -1 && sourceOfTarget[d] != s) { ok = false; break; }
                sourceOfTarget[d] = s;
            }
        }

        vector<int> ans(n, -1);
        if (ok) {
            for (int s = 0; s < m; ++s) if (target[s] != -1) {
                const auto &sc = cycles[s].nodes;
                const auto &dc = cycles[target[s]].nodes;
                for (int k = 0; k < (int)sc.size(); ++k)
                    ans[sc[k]] = dc[(k + shift[s]) % sc.size()] + 1;
            }

            map<int, set<int>> available;
            for (int d = 0; d < m; ++d) if (sourceOfTarget[d] == -1)
                for (int v : cycles[d].nodes) available[cycles[d].len].insert(v);

            vector<int> freeSources;
            for (int s = 0; s < m; ++s) if (target[s] == -1) freeSources.push_back(s);
            sort(freeSources.begin(), freeSources.end(), [&](int x, int y) {
                return cycles[x].anchor < cycles[y].anchor;
            });
            for (int s : freeSources) {
                int L = cycles[s].len;
                auto it = available.find(L);
                if (it == available.end() || it->second.empty()) { ok = false; break; }
                int anchor = cycles[s].anchor;
                int anchorPos = pos[anchor];
                int chosen = *it->second.begin();
                int d = cid[chosen];
                int sh = (pos[chosen] - anchorPos + L) % L;
                for (int k = 0; k < L; ++k)
                    ans[cycles[s].nodes[k]] = cycles[d].nodes[(k + sh) % L] + 1;
                for (int v : cycles[d].nodes) it->second.erase(v);
            }
        }

        if (!ok) {
            cout << "NO\n";
        } else {
            cout << "YES\n";
            for (int i = 0; i < n; ++i) cout << ans[i] << (i + 1 == n ? '\n' : ' ');
        }
    }
    return 0;
}
