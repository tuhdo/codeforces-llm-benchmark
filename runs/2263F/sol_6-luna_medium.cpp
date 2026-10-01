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
        vector<int> a(n), b(n);
        for (int &x : a) { cin >> x; --x; }
        for (int &x : b) { cin >> x; --x; }

        int changed = 0;
        for (int i = 0; i < n; ++i) changed += (a[i] != b[i]);
        if (changed == 0) {
            cout << "0 1\n\n";
            continue;
        }

        bool possible = true;
        for (int i = 0; i < n; ++i)
            if (a[i] > b[i]) possible = false;
        if (!possible) {
            cout << "-1\n";
            continue;
        }

        // Vertices encountered after a changed edge is finalized.
        vector<char> useful(n, false);
        for (int i = 0; i < n; ++i) if (a[i] != b[i]) {
            int v = i;
            while (!useful[v]) {
                useful[v] = true;
                v = b[v];
            }
        }
        vector<int> ends;
        for (int i = 0; i < n; ++i)
            if (useful[i] && b[i] == i) ends.push_back(i);
        if (ends.size() != 1) {
            cout << "-1\n";
            continue;
        }

        // The interval graph contains every destination a vertex can take
        // while still being possible to finish at its target b[i].
        vector<vector<int>> adj(n), radj(n);
        for (int i = 0; i < n; ++i) {
            for (int j = a[i]; j <= b[i]; ++j) {
                adj[i].push_back(j);
                radj[j].push_back(i);
            }
        }

        vector<int> disc(n, -1), low(n), compId(n, -1), st;
        int timer = 0, compsCount = 0;
        function<void(int)> tarjan = [&](int v) {
            disc[v] = low[v] = timer++;
            st.push_back(v);
            for (int u : adj[v]) {
                if (disc[u] == -1) {
                    tarjan(u);
                    low[v] = min(low[v], low[u]);
                } else if (compId[u] == -1) {
                    low[v] = min(low[v], disc[u]);
                }
            }
            if (low[v] == disc[v]) {
                while (true) {
                    int u = st.back(); st.pop_back();
                    compId[u] = compsCount;
                    if (u == v) break;
                }
                ++compsCount;
            }
        };
        for (int i = 0; i < n; ++i) if (disc[i] == -1) tarjan(i);

        vector<vector<int>> comps(compsCount);
        for (int i = 0; i < n; ++i) comps[compId[i]].push_back(i);
        vector<int> parent(compsCount, -1), need(compsCount, 0);
        for (int i = 0; i < n; ++i) {
            if (a[i] != b[i]) ++need[compId[i]];
            if (useful[i] && compId[i] != compId[b[i]]) {
                int to = compId[b[i]], from = compId[i];
                if (parent[to] != -1 && parent[to] != from) possible = false;
                parent[to] = from;
            }
        }

        vector<int> chain;
        int c = compId[ends[0]], sumNeed = 0;
        while (c != -1) {
            chain.push_back(c);
            sumNeed += need[c];
            c = parent[c];
        }
        if (sumNeed != changed) possible = false;
        if (!possible) {
            cout << "-1\n";
            continue;
        }
        reverse(chain.begin(), chain.end());

        vector<int> ops;
        int pos = comps[chain[0]][0];
        auto inc = [&]() { ops.push_back(1); ++a[pos]; };
        auto follow = [&]() { ops.push_back(2); pos = a[pos]; };

        for (int cid : chain) {
            const vector<int> &vertices = comps[cid];
            vector<int> towardRoot(n, -1);
            function<void(int)> buildTree = [&](int v) {
                for (int u : radj[v]) {
                    if (compId[u] == cid && towardRoot[u] == -1) {
                        towardRoot[u] = v;
                        buildTree(u);
                    }
                }
            };
            int root = vertices[0];
            buildTree(root);

            for (int target : vertices) {
                while (pos != target) {
                    int next = max(towardRoot[pos], target);
                    while (a[pos] < next) inc();
                    follow();
                }
                while (a[pos] < b[pos]) inc();
                follow();
            }
        }

        cout << ops.size() << ' ' << comps[chain[0]][0] + 1 << '\n';
        for (int op : ops) cout << op << ' ';
        cout << '\n';
    }
    return 0;
}
