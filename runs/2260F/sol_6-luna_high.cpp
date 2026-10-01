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

        // Build a spanning tree. Every remaining edge adds one independent
        // cycle, and there are at most ten such edges.
        vector<int> parent(n, -1), parent_edge(n, -1), depth(n, 0);
        vector<char> is_tree_edge(m, false);
        vector<int> order = {0};
        parent[0] = 0;
        for (int p = 0; p < (int)order.size(); ++p) {
            int u = order[p];
            for (auto [v, id] : graph[u]) {
                if (parent[v] != -1) continue;
                parent[v] = u;
                parent_edge[v] = id;
                depth[v] = depth[u] + 1;
                is_tree_edge[id] = true;
                order.push_back(v);
            }
        }

        vector<int> chords;
        for (int i = 0; i < m; ++i) {
            if (!is_tree_edge[i]) chords.push_back(i);
        }
        int k = (int)chords.size();

        // signature[e] is the set of fundamental cycles containing edge e.
        // A subset of cycle-space generators contains e exactly when the
        // corresponding parity is odd.
        vector<unsigned short> signature(m, 0);
        for (int bit = 0; bit < k; ++bit) {
            int chord = chords[bit];
            unsigned short flag = (unsigned short)(1u << bit);
            signature[chord] ^= flag;

            int u = edges[chord].u;
            int v = edges[chord].v;
            while (depth[u] > depth[v]) {
                signature[parent_edge[u]] ^= flag;
                u = parent[u];
            }
            while (depth[v] > depth[u]) {
                signature[parent_edge[v]] ^= flag;
                v = parent[v];
            }
            while (u != v) {
                signature[parent_edge[u]] ^= flag;
                signature[parent_edge[v]] ^= flag;
                u = parent[u];
                v = parent[v];
            }
        }

        bool possible = false;
        vector<int> selected_degree(n);
        vector<char> selected(m);
        vector<char> seen(n);

        for (int mask = 1; mask < (1 << k) && !possible; ++mask) {
            fill(selected_degree.begin(), selected_degree.end(), 0);
            int selected_count = 0;
            for (int i = 0; i < m; ++i) {
                selected[i] = (__builtin_parity((unsigned)(signature[i] & mask)) != 0);
                if (selected[i]) {
                    ++selected_count;
                    ++selected_degree[edges[i].u];
                    ++selected_degree[edges[i].v];
                }
            }
            if (selected_count < 3) continue;

            int start = -1;
            bool is_one_cycle = true;
            for (int v = 0; v < n; ++v) {
                if (selected_degree[v] != 0 && selected_degree[v] != 2) {
                    is_one_cycle = false;
                    break;
                }
                if (selected_degree[v] == 2 && start == -1) start = v;
            }
            if (!is_one_cycle) continue;

            // A 2-regular edge set may be a disjoint union of cycles.
            fill(seen.begin(), seen.end(), false);
            vector<int> stack = {start};
            seen[start] = true;
            while (!stack.empty()) {
                int u = stack.back();
                stack.pop_back();
                for (auto [v, id] : graph[u]) {
                    if (selected[id] && !seen[v]) {
                        seen[v] = true;
                        stack.push_back(v);
                    }
                }
            }
            for (int v = 0; v < n; ++v) {
                if (selected_degree[v] != 0 && !seen[v]) {
                    is_one_cycle = false;
                    break;
                }
            }
            if (!is_one_cycle) continue;

            // Removing this cycle must leave a connected spanning graph.
            fill(seen.begin(), seen.end(), false);
            stack = {0};
            seen[0] = true;
            while (!stack.empty()) {
                int u = stack.back();
                stack.pop_back();
                for (auto [v, id] : graph[u]) {
                    if (!selected[id] && !seen[v]) {
                        seen[v] = true;
                        stack.push_back(v);
                    }
                }
            }
            possible = all_of(seen.begin(), seen.end(), [](char x) { return x; });
        }

        cout << (possible ? "YES\n" : "NO\n");
    }
    return 0;
}
