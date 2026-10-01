#include <bits/stdc++.h>
using namespace std;

// For a color appearing m times, an edge splitting its vertices into a and
// m-a has weight min(a, m-a). Rooting at a median makes these weights equal
// to subtree counts, so they never increase along a path away from the root.
// The largest k-1 weights can therefore form a connected component, whose
// cost is the sum of all edge weights minus those k-1 weights.

class Tree {
    static constexpr int LOG = 19;
    vector<array<int, LOG>> up;

public:
    vector<vector<int>> adj;
    vector<int> depth, tin, tout, order;

    explicit Tree(int n)
        : up(n + 1), adj(n + 1), depth(n + 1),
          tin(n + 1), tout(n + 1) {
        order.reserve(n);
    }

    void build() {
        vector<int> stack = {1};
        up[1].fill(1);
        while (!stack.empty()) {
            int v = stack.back();
            stack.pop_back();
            tin[v] = static_cast<int>(order.size());
            order.push_back(v);
            for (int u : adj[v]) {
                if (u == up[v][0]) continue;
                depth[u] = depth[v] + 1;
                up[u][0] = v;
                for (int j = 1; j < LOG; ++j) {
                    up[u][j] = up[up[u][j - 1]][j - 1];
                }
                stack.push_back(u);
            }
        }

        vector<int> size(adj.size(), 1);
        for (int i = static_cast<int>(order.size()) - 1; i >= 0; --i) {
            int v = order[i];
            tout[v] = tin[v] + size[v];
            if (v != 1) size[up[v][0]] += size[v];
        }
    }

    bool ancestor(int u, int v) const {
        return tin[u] <= tin[v] && tin[v] < tout[u];
    }

    int lca(int u, int v) const {
        if (ancestor(u, v)) return u;
        if (ancestor(v, u)) return v;
        for (int j = LOG - 1; j >= 0; --j) {
            if (!ancestor(up[u][j], v)) u = up[u][j];
        }
        return up[u][0];
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        vector<int> color(n + 1), k(n + 1);
        for (int v = 1; v <= n; ++v) cin >> color[v];
        for (int c = 1; c <= n; ++c) cin >> k[c];

        Tree tree(n);
        for (int i = 1; i < n; ++i) {
            int u, v;
            cin >> u >> v;
            tree.adj[u].push_back(v);
            tree.adj[v].push_back(u);
        }
        tree.build();

        // Each color's vertices are already in DFS order.
        vector<vector<int>> vertices(n + 1);
        for (int v : tree.order) vertices[color[v]].push_back(v);

        vector<long long> answer(n + 1, -1);
        vector<int> nodes, parent, count, stack;
        vector<pair<int, int>> weights;
        for (int c = 1; c <= n; ++c) {
            const auto& marked = vertices[c];
            int m = static_cast<int>(marked.size());
            if (m == 0) continue;
            if (m == 1) {
                answer[c] = 0;
                continue;
            }

            nodes.assign(marked.begin(), marked.end());
            for (int i = 1; i < m; ++i) {
                nodes.push_back(tree.lca(marked[i - 1], marked[i]));
            }
            sort(nodes.begin(), nodes.end(), [&](int u, int v) {
                return tree.tin[u] < tree.tin[v];
            });
            nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());

            int sz = static_cast<int>(nodes.size());
            parent.assign(sz, -1);
            count.resize(sz);
            stack.clear();
            for (int i = 0; i < sz; ++i) {
                count[i] = (color[nodes[i]] == c);
                while (!stack.empty() &&
                       !tree.ancestor(nodes[stack.back()], nodes[i])) {
                    stack.pop_back();
                }
                if (!stack.empty()) parent[i] = stack.back();
                stack.push_back(i);
            }

            long long cost = 0;
            weights.clear();
            for (int i = sz - 1; i > 0; --i) {
                int p = parent[i];
                int length = tree.depth[nodes[i]] - tree.depth[nodes[p]];
                int weight = min(count[i], m - count[i]);
                // Every original edge on this virtual edge has this weight.
                cost += 1LL * weight * length;
                weights.emplace_back(weight, length);
                count[p] += count[i];
            }

            int remaining = k[c] - 1;
            if (remaining > 0) {
                sort(weights.begin(), weights.end(), greater<pair<int, int>>());
                for (auto [weight, length] : weights) {
                    int take = min(remaining, length);
                    cost -= 1LL * weight * take;
                    remaining -= take;
                    if (remaining == 0) break;
                }
            }
            answer[c] = cost;
        }

        for (int c = 1; c <= n; ++c) {
            if (c > 1) cout << ' ';
            cout << answer[c];
        }
        cout << '\n';
    }
    return 0;
}
