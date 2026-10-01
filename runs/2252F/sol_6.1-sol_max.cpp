#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

constexpr int LOG = 20;

// Give an edge weight min(s, m-s), where s color vertices lie on one side.
// Each edge outside a chosen component contributes at least this weight.
// Root at a vertex whose branches each contain at most m/2 color vertices.
// Weights then never increase along a path away from the root, so the k-1
// largest weights can form a connected component together with the root.
// Its cost is the sum of all weights minus those k-1 largest weights.
// A virtual tree groups paths whose edges all have the same weight.
void solve() {
    int n;
    cin >> n;

    vector<int> color(n), target(n + 1);
    for (int &c : color) cin >> c;
    for (int c = 1; c <= n; ++c) cin >> target[c];

    vector<vector<int>> graph(n);
    for (int i = 1; i < n; ++i) {
        int u, v;
        cin >> u >> v;
        --u;
        --v;
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    vector<int> parent(n), depth(n), entry(n), subtree_size(n, 1);
    vector<array<int, LOG>> jump(n);
    vector<vector<int>> colored_vertices(n + 1);
    vector<int> order, traversal_stack;
    order.reserve(n);
    traversal_stack.reserve(n);
    traversal_stack.push_back(0);

    while (!traversal_stack.empty()) {
        int u = traversal_stack.back();
        traversal_stack.pop_back();
        entry[u] = static_cast<int>(order.size());
        order.push_back(u);
        colored_vertices[color[u]].push_back(u);

        jump[u][0] = parent[u];
        for (int j = 1; j < LOG; ++j) {
            jump[u][j] = jump[jump[u][j - 1]][j - 1];
        }
        for (int v : graph[u]) {
            if (v == parent[u]) continue;
            parent[v] = u;
            depth[v] = depth[u] + 1;
            traversal_stack.push_back(v);
        }
    }

    for (int i = n - 1; i > 0; --i) {
        int u = order[i];
        subtree_size[parent[u]] += subtree_size[u];
    }

    auto is_ancestor = [&](int u, int v) {
        return entry[u] <= entry[v] && entry[v] < entry[u] + subtree_size[u];
    };
    auto lca = [&](int u, int v) {
        if (is_ancestor(u, v)) return u;
        if (is_ancestor(v, u)) return v;
        for (int j = LOG - 1; j >= 0; --j) {
            if (!is_ancestor(jump[u][j], v)) u = jump[u][j];
        }
        return parent[u];
    };

    vector<long long> answer(n + 1, -1);
    vector<int> nodes, virtual_parent, count, ancestor_stack;
    vector<pair<int, int>> edge_groups;

    for (int c = 1; c <= n; ++c) {
        const auto &vertices = colored_vertices[c];
        int total = static_cast<int>(vertices.size());
        if (total == 0) continue;
        if (total == 1) {
            answer[c] = 0;
            continue;
        }

        nodes.assign(vertices.begin(), vertices.end());
        for (int i = 1; i < total; ++i) {
            nodes.push_back(lca(vertices[i - 1], vertices[i]));
        }
        sort(nodes.begin(), nodes.end(), [&](int u, int v) {
            return entry[u] < entry[v];
        });
        nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());

        int size = static_cast<int>(nodes.size());
        virtual_parent.resize(size);
        count.resize(size);
        ancestor_stack.clear();
        for (int i = 0; i < size; ++i) {
            while (!ancestor_stack.empty() &&
                   !is_ancestor(nodes[ancestor_stack.back()], nodes[i])) {
                ancestor_stack.pop_back();
            }
            virtual_parent[i] = ancestor_stack.empty() ? -1 : ancestor_stack.back();
            count[i] = (color[nodes[i]] == c);
            ancestor_stack.push_back(i);
        }

        long long cost = 0;
        edge_groups.clear();
        for (int i = size - 1; i > 0; --i) {
            int p = virtual_parent[i];
            int weight = min(count[i], total - count[i]);
            int length = depth[nodes[i]] - depth[nodes[p]];

            // Every original edge on this compressed path has the same weight.
            cost += 1LL * weight * length;
            edge_groups.emplace_back(weight, length);
            count[p] += count[i];
        }

        int remaining = target[c] - 1;
        if (remaining > 0) {
            sort(edge_groups.begin(), edge_groups.end(), greater<pair<int, int>>());
            for (auto [weight, length] : edge_groups) {
                int take = min(remaining, length);
                cost -= 1LL * take * weight;
                remaining -= take;
                if (remaining == 0) break;
            }
        }
        answer[c] = cost;
    }

    for (int c = 1; c <= n; ++c) {
        cout << answer[c] << (c == n ? '\n' : ' ');
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) solve();
    return 0;
}
