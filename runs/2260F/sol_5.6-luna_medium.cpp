#include <bits/stdc++.h>
using namespace std;

struct DSU {
    vector<int> p, sz;
    explicit DSU(int n) : p(n), sz(n, 1) { iota(p.begin(), p.end(), 0); }
    int find(int x) { return p[x] == x ? x : p[x] = find(p[x]); }
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (sz[a] < sz[b]) swap(a, b);
        p[b] = a;
        sz[a] += sz[b];
        return true;
    }
};

struct Edge {
    int u, v;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n, m;
        cin >> n >> m;
        vector<Edge> edges(m);
        for (auto &e : edges) {
            cin >> e.u >> e.v;
            --e.u;
            --e.v;
        }

        DSU dsu(n);
        vector<int> treeEdges;
        vector<int> extraEdges;
        vector<vector<pair<int, int>>> tree(n);
        vector<vector<pair<int, int>>> graph(n);
        for (int i = 0; i < m; ++i) {
            graph[edges[i].u].push_back({edges[i].v, i});
            graph[edges[i].v].push_back({edges[i].u, i});
            if (dsu.unite(edges[i].u, edges[i].v)) {
                treeEdges.push_back(i);
                tree[edges[i].u].push_back({edges[i].v, i});
                tree[edges[i].v].push_back({edges[i].u, i});
            } else {
                extraEdges.push_back(i);
            }
        }

        const int k = (int)extraEdges.size();
        const int words = (m + 63) / 64;
        vector<vector<unsigned long long>> fundamental(
            k, vector<unsigned long long>(words, 0));

        for (int j = 0; j < k; ++j) {
            int edgeId = extraEdges[j];
            int start = edges[edgeId].u;
            int target = edges[edgeId].v;

            vector<int> parent(n, -1), parentEdge(n, -1);
            queue<int> q;
            q.push(start);
            parent[start] = start;
            while (!q.empty()) {
                int v = q.front();
                q.pop();
                if (v == target) break;
                for (auto [to, treeEdgeId] : tree[v]) {
                    if (parent[to] != -1) continue;
                    parent[to] = v;
                    parentEdge[to] = treeEdgeId;
                    q.push(to);
                }
            }

            auto &mask = fundamental[j];
            auto addEdge = [&](int id) { mask[id >> 6] ^= 1ULL << (id & 63); };
            addEdge(edgeId);
            for (int v = target; v != start; v = parent[v]) {
                addEdge(parentEdge[v]);
            }
        }

        bool possible = false;
        const int totalMasks = 1 << k;
        for (int subset = 1; subset < totalMasks && !possible; ++subset) {
            vector<unsigned long long> cycle(words, 0);
            for (int bit = 0; bit < k; ++bit) {
                if (!(subset & (1 << bit))) continue;
                for (int w = 0; w < words; ++w) {
                    cycle[w] ^= fundamental[bit][w];
                }
            }

            vector<int> degree(n, 0);
            int usedVertices = 0;
            bool valid = true;
            for (int i = 0; i < m; ++i) {
                if (!(cycle[i >> 6] & (1ULL << (i & 63)))) continue;
                ++degree[edges[i].u];
                ++degree[edges[i].v];
            }
            for (int v = 0; v < n; ++v) {
                if (degree[v] == 0) continue;
                ++usedVertices;
                if (degree[v] != 2) valid = false;
            }
            if (!valid || usedVertices < 3) continue;

            int start = -1;
            for (int v = 0; v < n; ++v) {
                if (degree[v]) {
                    start = v;
                    break;
                }
            }
            vector<char> seen(n, false);
            queue<int> q;
            q.push(start);
            seen[start] = true;
            while (!q.empty()) {
                int v = q.front();
                q.pop();
                for (auto [to, i] : graph[v]) {
                    if (!(cycle[i >> 6] & (1ULL << (i & 63)))) continue;
                    if (!seen[to]) {
                        seen[to] = true;
                        q.push(to);
                    }
                }
            }
            for (int v = 0; v < n; ++v) {
                if (degree[v] && !seen[v]) valid = false;
            }
            if (!valid) continue;

            fill(seen.begin(), seen.end(), false);
            q.push(0);
            seen[0] = true;
            while (!q.empty()) {
                int v = q.front();
                q.pop();
                for (auto [to, i] : graph[v]) {
                    if (cycle[i >> 6] & (1ULL << (i & 63))) continue;
                    if (!seen[to]) {
                        seen[to] = true;
                        q.push(to);
                    }
                }
            }
            if (all_of(seen.begin(), seen.end(), [](char x) { return x; })) {
                possible = true;
            }
        }

        cout << (possible ? "YES\n" : "NO\n");
    }
}
