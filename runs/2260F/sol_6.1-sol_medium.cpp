#include <bits/stdc++.h>
using namespace std;

void solve() {
    int n, m;
    cin >> n >> m;
    vector<pair<int, int>> edges(m);
    vector<vector<pair<int, int>>> adj(n);
    for (int i = 0; i < m; ++i) {
        auto &[u, v] = edges[i];
        cin >> u >> v;
        --u;
        --v;
        adj[u].push_back({v, i});
        adj[v].push_back({u, i});
    }

    vector<int> parent(n, -1), parentEdge(n, -1), depth(n);
    vector<bool> treeEdge(m, false);
    vector<int> queue(n);
    parent[0] = 0;
    queue[0] = 0;
    int tail = 1;
    for (int head = 0; head < tail; ++head) {
        int u = queue[head];
        for (auto [v, id] : adj[u]) {
            if (parent[v] != -1) continue;
            parent[v] = u;
            parentEdge[v] = id;
            depth[v] = depth[u] + 1;
            treeEdge[id] = true;
            queue[tail++] = v;
        }
    }

    // Bit k marks membership in the k-th fundamental cycle.
    vector<int> mask(m, 0);
    int r = 0;
    for (int id = 0; id < m; ++id) {
        if (treeEdge[id]) continue;
        int bit = 1 << r++;
        mask[id] = bit;
        auto [u, v] = edges[id];
        while (u != v) {
            if (depth[u] < depth[v]) swap(u, v);
            mask[parentEdge[u]] |= bit;
            u = parent[u];
        }
    }

    vector<int> seen(n, 0);
    for (int subset = 1; subset < (1 << r); ++subset) {
        queue[0] = 0;
        seen[0] = subset;
        tail = 1;
        for (int head = 0; head < tail; ++head) {
            int u = queue[head];
            for (auto [v, id] : adj[u]) {
                if (seen[v] == subset) continue;
                if (__builtin_parity((unsigned)(mask[id] & subset))) continue;
                seen[v] = subset;
                queue[tail++] = v;
            }
        }
        if (tail == n) {
            cout << "YES\n";
            return;
        }
    }
    cout << "NO\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    cin >> t;
    while (t--) solve();
}
