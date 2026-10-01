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
        vector<int> color(n), k(n);
        vector<vector<int>> groups(n), adj(n);
        for (int v = 0; v < n; ++v) {
            cin >> color[v];
            --color[v];
            groups[color[v]].push_back(v);
        }
        for (int &x : k) cin >> x;
        for (int i = 1; i < n; ++i) {
            int u, v;
            cin >> u >> v;
            --u;
            --v;
            adj[u].push_back(v);
            adj[v].push_back(u);
        }

        constexpr int LOG = 20;
        vector<array<int, LOG>> up(n);
        vector<int> parent(n, -1), depth(n), tin(n), tout(n), size(n, 1);
        vector<int> order, dfs = {0};
        parent[0] = 0;
        while (!dfs.empty()) {
            int v = dfs.back();
            dfs.pop_back();
            tin[v] = (int)order.size();
            order.push_back(v);
            up[v][0] = parent[v];
            for (int j = 1; j < LOG; ++j)
                up[v][j] = up[up[v][j - 1]][j - 1];
            for (int u : adj[v]) {
                if (u == parent[v]) continue;
                parent[u] = v;
                depth[u] = depth[v] + 1;
                dfs.push_back(u);
            }
        }
        for (int i = n - 1; i >= 0; --i) {
            int v = order[i];
            tout[v] = tin[v] + size[v];
            if (v != 0) size[parent[v]] += size[v];
        }
        auto ancestor = [&](int u, int v) {
            return tin[u] <= tin[v] && tin[v] < tout[u];
        };
        auto lca = [&](int u, int v) {
            if (ancestor(u, v)) return u;
            if (ancestor(v, u)) return v;
            for (int j = LOG - 1; j >= 0; --j)
                if (!ancestor(up[u][j], v)) u = up[u][j];
            return up[u][0];
        };
        auto by_tin = [&](int u, int v) { return tin[u] < tin[v]; };

        vector<long long> answer(n, -1);
        for (int c = 0; c < n; ++c) {
            int m = (int)groups[c].size();
            if (m == 0) continue;
            if (m == 1) {
                answer[c] = 0;
                continue;
            }

            vector<int> vertices = groups[c];
            sort(vertices.begin(), vertices.end(), by_tin);
            for (int i = 1; i < m; ++i)
                vertices.push_back(lca(vertices[i - 1], vertices[i]));
            sort(vertices.begin(), vertices.end(), by_tin);
            vertices.erase(unique(vertices.begin(), vertices.end()), vertices.end());

            int q = (int)vertices.size();
            vector<int> virtual_parent(q, -1), count(q), stack;
            for (int i = 0; i < q; ++i) {
                while (!stack.empty() && !ancestor(vertices[stack.back()], vertices[i]))
                    stack.pop_back();
                if (!stack.empty()) virtual_parent[i] = stack.back();
                stack.push_back(i);
                count[i] = (color[vertices[i]] == c);
            }

            long long cost = 0;
            vector<pair<int, int>> gains;
            gains.reserve(q - 1);
            for (int i = q - 1; i > 0; --i) {
                int p = virtual_parent[i];
                int length = depth[vertices[i]] - depth[vertices[p]];
                int gain = min(count[i], m - count[i]);
                cost += 1LL * gain * length;
                gains.emplace_back(gain, length);
                count[p] += count[i];
            }

            sort(gains.begin(), gains.end(), greater<pair<int, int>>());
            int remaining = k[c] - 1;
            for (auto [gain, length] : gains) {
                int take = min(remaining, length);
                cost -= 1LL * gain * take;
                remaining -= take;
                if (remaining == 0) break;
            }
            answer[c] = cost;
        }

        for (int c = 0; c < n; ++c)
            cout << answer[c] << (c + 1 == n ? '\n' : ' ');
    }
}
