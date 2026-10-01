#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        vector<int> color(n), k(n + 1);
        vector<vector<int>> by_color(n + 1), graph(n);
        for (int v = 0; v < n; ++v) {
            cin >> color[v];
            by_color[color[v]].push_back(v);
        }
        for (int c = 1; c <= n; ++c) cin >> k[c];
        for (int i = 0; i < n - 1; ++i) {
            int u, v;
            cin >> u >> v;
            --u;
            --v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }

        int log = 1;
        while ((1 << log) <= n) ++log;
        vector<vector<int>> up(log, vector<int>(n));
        vector<int> parent(n), depth(n), tin(n), tout(n), size(n, 1);
        vector<int> order, stack;
        order.reserve(n);
        stack.reserve(n);
        stack.push_back(0);
        while (!stack.empty()) {
            int v = stack.back();
            stack.pop_back();
            tin[v] = (int)order.size();
            order.push_back(v);
            up[0][v] = parent[v];
            for (int j = 1; j < log; ++j) {
                up[j][v] = up[j - 1][up[j - 1][v]];
            }
            for (int u : graph[v]) {
                if (u == parent[v]) continue;
                parent[u] = v;
                depth[u] = depth[v] + 1;
                stack.push_back(u);
            }
        }
        for (int i = n - 1; i > 0; --i) {
            int v = order[i];
            size[parent[v]] += size[v];
        }
        for (int v = 0; v < n; ++v) tout[v] = tin[v] + size[v];

        auto ancestor = [&](int u, int v) {
            return tin[u] <= tin[v] && tin[v] < tout[u];
        };
        auto lca = [&](int u, int v) {
            if (ancestor(u, v)) return u;
            if (ancestor(v, u)) return v;
            for (int j = log - 1; j >= 0; --j) {
                if (!ancestor(up[j][u], v)) u = up[j][u];
            }
            return up[0][u];
        };
        auto by_entry = [&](int u, int v) { return tin[u] < tin[v]; };

        vector<long long> answer(n + 1, -1);
        for (int c = 1; c <= n; ++c) {
            int m = (int)by_color[c].size();
            if (m == 0) continue;
            if (m == 1) {
                answer[c] = 0;
                continue;
            }

            vector<int> nodes = by_color[c];
            nodes.reserve(2 * m);
            sort(nodes.begin(), nodes.end(), by_entry);
            for (int i = 1; i < m; ++i) {
                nodes.push_back(lca(nodes[i - 1], nodes[i]));
            }
            sort(nodes.begin(), nodes.end(), by_entry);
            nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());

            int s = (int)nodes.size();
            vector<int> virtual_parent(s, -1), count(s);
            stack.clear();
            for (int i = 0; i < s; ++i) {
                while (!stack.empty() && !ancestor(nodes[stack.back()], nodes[i])) {
                    stack.pop_back();
                }
                if (!stack.empty()) virtual_parent[i] = stack.back();
                count[i] = (color[nodes[i]] == c);
                stack.push_back(i);
            }
            for (int i = s - 1; i > 0; --i) {
                count[virtual_parent[i]] += count[i];
            }

            // The deepest subtree containing more than half the terminals
            // has no incident side containing more than half: it is a median.
            int median = 0;
            long long cost = 0;
            for (int i = 1; i < s; ++i) {
                int length = depth[nodes[i]] - depth[nodes[virtual_parent[i]]];
                cost += 1LL * count[i] * length;
                if (2 * count[i] > m && depth[nodes[i]] > depth[nodes[median]]) {
                    median = i;
                }
            }
            for (int i = median; virtual_parent[i] != -1; i = virtual_parent[i]) {
                int length = depth[nodes[i]] - depth[nodes[virtual_parent[i]]];
                cost += 1LL * (m - 2 * count[i]) * length;
            }

            vector<pair<int, int>> savings;
            savings.reserve(s - 1);
            for (int i = 1; i < s; ++i) {
                int length = depth[nodes[i]] - depth[nodes[virtual_parent[i]]];
                int weight = ancestor(nodes[i], nodes[median]) ? m - count[i] : count[i];
                // All original edges on this compressed path have this weight.
                savings.emplace_back(weight, length);
            }
            sort(savings.begin(), savings.end(), greater<pair<int, int>>());
            int remaining = k[c] - 1;
            for (auto [weight, length] : savings) {
                int take = min(remaining, length);
                cost -= 1LL * take * weight;
                remaining -= take;
                if (remaining == 0) break;
            }
            answer[c] = cost;
        }

        for (int c = 1; c <= n; ++c) {
            cout << answer[c] << (c == n ? '\n' : ' ');
        }
    }
}
