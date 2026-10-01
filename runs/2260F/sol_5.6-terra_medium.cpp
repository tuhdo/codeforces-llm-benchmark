#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int to, id;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n, m;
        cin >> n >> m;
        vector<int> a(m), b(m);
        vector<vector<Edge>> g(n);
        for (int i = 0; i < m; ++i) {
            cin >> a[i] >> b[i];
            --a[i]; --b[i];
            g[a[i]].push_back({b[i], i});
            g[b[i]].push_back({a[i], i});
        }

        // A color-2 path P and a color-1 edge together with a color-3 tree
        // spanning P have |P|+1 independent cycles.  Thus |P| <= m-n.
        const int limit = m - n;
        if (limit <= 0) {
            cout << "NO\n";
            continue;
        }

        // An edge of either of the first two colors cannot be a bridge.
        vector<int> tin(n, -1), low(n), bridge(m, false);
        int timer = 0;
        auto find_bridges = [&](auto&& self, int v, int parent_edge) -> void {
            tin[v] = low[v] = timer++;
            for (auto [to, id] : g[v]) {
                if (id == parent_edge) continue;
                if (tin[to] != -1) {
                    low[v] = min(low[v], tin[to]);
                } else {
                    self(self, to, id);
                    low[v] = min(low[v], low[to]);
                    if (low[to] > tin[v]) bridge[id] = true;
                }
            }
        };
        find_bridges(find_bridges, 0, -1);

        bool answer = false;
        vector<char> used_vertex(n), removed(m);

        auto remaining_connects_path = [&](const vector<int>& path_edges,
                                          const vector<int>& path_vertices,
                                          int first_edge) {
            fill(removed.begin(), removed.end(), false);
            removed[first_edge] = true;
            for (int id : path_edges) removed[id] = true;

            vector<char> seen(n);
            queue<int> q;
            q.push(path_vertices[0]);
            seen[path_vertices[0]] = true;
            while (!q.empty()) {
                int v = q.front(); q.pop();
                for (auto [to, id] : g[v]) {
                    if (!removed[id] && !seen[to]) {
                        seen[to] = true;
                        q.push(to);
                    }
                }
            }
            for (int v : path_vertices)
                if (!seen[v]) return false;
            return true;
        };

        for (int first = 0; first < m && !answer; ++first) {
            if (bridge[first]) continue;
            int source = a[first], target = b[first];
            vector<int> path_edges, path_vertices{source};
            fill(used_vertex.begin(), used_vertex.end(), false);
            used_vertex[source] = true;

            auto dfs = [&](auto&& self, int v, int depth) -> void {
                if (answer || depth == limit) return;
                for (auto [to, id] : g[v]) {
                    if (answer || id == first || bridge[id] || used_vertex[to]) continue;
                    path_edges.push_back(id);
                    path_vertices.push_back(to);
                    if (to == target) {
                        if (remaining_connects_path(path_edges, path_vertices, first))
                            answer = true;
                    } else {
                        used_vertex[to] = true;
                        self(self, to, depth + 1);
                        used_vertex[to] = false;
                    }
                    path_vertices.pop_back();
                    path_edges.pop_back();
                }
            };
            dfs(dfs, source, 0);
        }
        cout << (answer ? "YES\n" : "NO\n");
    }
}
