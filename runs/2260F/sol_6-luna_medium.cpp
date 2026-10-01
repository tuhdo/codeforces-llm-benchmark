#include <bits/stdc++.h>
using namespace std;

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
        vector<vector<pair<int, int>>> graph(n);
        for (int i = 0; i < m; ++i) {
            int u, v;
            cin >> u >> v;
            --u;
            --v;
            edges[i] = {u, v};
            graph[u].push_back({v, i});
            graph[v].push_back({u, i});
        }

        // A DFS spanning tree gives one fundamental cycle per non-tree edge.
        vector<int> parent(n, -1), parentEdge(n, -1), depth(n, 0);
        vector<char> seen(n, false), isTreeEdge(m, false);
        vector<int> order{0};
        seen[0] = true;
        for (size_t at = 0; at < order.size(); ++at) {
            int u = order[at];
            for (auto [v, id] : graph[u]) {
                if (seen[v]) continue;
                seen[v] = true;
                parent[v] = u;
                parentEdge[v] = id;
                depth[v] = depth[u] + 1;
                isTreeEdge[id] = true;
                order.push_back(v);
            }
        }

        vector<int> chords;
        for (int i = 0; i < m; ++i)
            if (!isTreeEdge[i]) chords.push_back(i);

        bool possible = false;
        int k = (int)chords.size();
        // The cycle rank m-n+1 is at most 10.
        for (int mask = 1; mask < (1 << k) && !possible; ++mask) {
            vector<char> removed(m, false);
            for (int j = 0; j < k; ++j) {
                if (!(mask & (1 << j))) continue;
                int id = chords[j];
                removed[id] = true;
                int u = edges[id].u, v = edges[id].v;
                while (depth[u] > depth[v]) {
                    removed[parentEdge[u]] = true;
                    u = parent[u];
                }
                while (depth[v] > depth[u]) {
                    removed[parentEdge[v]] = true;
                    v = parent[v];
                }
                while (u != v) {
                    removed[parentEdge[u]] = true;
                    removed[parentEdge[v]] = true;
                    u = parent[u];
                    v = parent[v];
                }
            }

            vector<char> reached(n, false);
            vector<int> stack{0};
            reached[0] = true;
            int count = 0;
            while (!stack.empty()) {
                int u = stack.back();
                stack.pop_back();
                ++count;
                for (auto [v, id] : graph[u]) {
                    if (removed[id] || reached[v]) continue;
                    reached[v] = true;
                    stack.push_back(v);
                }
            }
            if (count == n) possible = true;
        }

        cout << (possible ? "YES\n" : "NO\n");
    }
    return 0;
}
