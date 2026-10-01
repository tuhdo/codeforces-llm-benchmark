#include <bits/stdc++.h>

using namespace std;

using int64 = long long;

struct Fenwick {
    int n;
    vector<int64> count;
    vector<int64> sum;

    explicit Fenwick(int n) : n(n), count(n + 1, 0), sum(n + 1, 0) {}

    void add(int index, int64 delta_count) {
        if (index <= 0 || index > n) return;
        for (int i = index; i <= n; i += i & -i) {
            count[i] += delta_count;
            sum[i] += delta_count * index;
        }
    }

    int64 prefix_count(int index) const {
        int64 result = 0;
        for (int i = index; i > 0; i -= i & -i) result += count[i];
        return result;
    }

    int64 prefix_sum(int index) const {
        int64 result = 0;
        for (int i = index; i > 0; i -= i & -i) result += sum[i];
        return result;
    }

    int kth(int64 order) const {
        int index = 0;
        int64 accumulated = 0;
        int step = 1;
        while ((step << 1) <= n) step <<= 1;
        for (; step > 0; step >>= 1) {
            int next = index + step;
            if (next <= n && accumulated + count[next] < order) {
                index = next;
                accumulated += count[next];
            }
        }
        return index + 1;
    }

    int64 largest_sum(int64 wanted) const {
        if (wanted <= 0 || n == 0) return 0;

        int64 total_count = prefix_count(n);
        int64 total_sum = prefix_sum(n);
        int64 take = min(wanted, total_count);
        if (take == 0) return 0;
        if (take == total_count) return total_sum;

        int64 first_order = total_count - take + 1;
        int value = kth(first_order);
        int64 count_not_greater = prefix_count(value);
        int64 sum_not_greater = prefix_sum(value);
        int64 count_greater = total_count - count_not_greater;
        int64 sum_greater = total_sum - sum_not_greater;
        return sum_greater + (take - count_greater) * value;
    }
};

struct VirtualEdge {
    int parent;
    int child;
    int length;
    int side_count;
};

struct VirtualAdjEdge {
    int to;
    int id;
};

struct Event {
    int type;
    int node;
    int edge;
    int old_value;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int test_count;
    cin >> test_count;

    while (test_count--) {
        int n;
        cin >> n;

        vector<int> color(n + 1);
        vector<vector<int>> vertices_by_color(n + 1);
        for (int v = 1; v <= n; ++v) {
            cin >> color[v];
            vertices_by_color[color[v]].push_back(v);
        }

        vector<int> k(n + 1);
        for (int c = 1; c <= n; ++c) cin >> k[c];

        vector<vector<int>> graph(n + 1);
        for (int i = 0; i + 1 < n; ++i) {
            int u, v;
            cin >> u >> v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }

        int log_n = 1;
        while ((1 << log_n) <= n) ++log_n;
        vector<vector<int>> up(log_n, vector<int>(n + 1));
        vector<int> depth(n + 1), tin(n + 1), tout(n + 1);

        struct DfsFrame {
            int node;
            int parent;
            int next_neighbor;
        };

        int timer = 0;
        vector<DfsFrame> dfs;
        dfs.push_back({1, 1, 0});
        up[0][1] = 1;
        tin[1] = ++timer;

        while (!dfs.empty()) {
            DfsFrame &frame = dfs.back();
            if (frame.next_neighbor == static_cast<int>(graph[frame.node].size())) {
                tout[frame.node] = timer;
                dfs.pop_back();
                continue;
            }

            int to = graph[frame.node][frame.next_neighbor++];
            if (to == frame.parent) continue;

            up[0][to] = frame.node;
            depth[to] = depth[frame.node] + 1;
            tin[to] = ++timer;
            dfs.push_back({to, frame.node, 0});
        }

        for (int j = 1; j < log_n; ++j) {
            for (int v = 1; v <= n; ++v) {
                up[j][v] = up[j - 1][up[j - 1][v]];
            }
        }

        auto is_ancestor = [&](int u, int v) {
            return tin[u] <= tin[v] && tout[v] <= tout[u];
        };

        auto lca = [&](int u, int v) {
            if (is_ancestor(u, v)) return u;
            if (is_ancestor(v, u)) return v;
            int x = u;
            for (int j = log_n - 1; j >= 0; --j) {
                if (!is_ancestor(up[j][x], v)) x = up[j][x];
            }
            return up[0][x];
        };

        vector<int64> answer(n + 1, -1);

        for (int c = 1; c <= n; ++c) {
            const vector<int> &terminals = vertices_by_color[c];
            int m = static_cast<int>(terminals.size());
            if (m == 0) continue;
            if (m == 1) {
                answer[c] = 0;
                continue;
            }

            vector<int> ordered_terminals = terminals;
            sort(ordered_terminals.begin(), ordered_terminals.end(),
                 [&](int a, int b) { return tin[a] < tin[b]; });

            vector<int> virtual_nodes = ordered_terminals;
            virtual_nodes.reserve(2 * m);
            for (int i = 1; i < m; ++i) {
                virtual_nodes.push_back(lca(ordered_terminals[i - 1], ordered_terminals[i]));
            }
            sort(virtual_nodes.begin(), virtual_nodes.end(),
                 [&](int a, int b) { return tin[a] < tin[b]; });
            virtual_nodes.erase(unique(virtual_nodes.begin(), virtual_nodes.end()),
                                virtual_nodes.end());

            int virtual_size = static_cast<int>(virtual_nodes.size());

            vector<int> parent(virtual_size, -1);
            vector<int> subtree_terminals(virtual_size, 0);
            vector<VirtualEdge> edges;
            edges.reserve(virtual_size - 1);
            vector<vector<VirtualAdjEdge>> virtual_graph(virtual_size);

            vector<int> ancestor_stack;
            ancestor_stack.reserve(virtual_size);
            for (int i = 0; i < virtual_size; ++i) {
                while (!ancestor_stack.empty() &&
                       !is_ancestor(virtual_nodes[ancestor_stack.back()], virtual_nodes[i])) {
                    ancestor_stack.pop_back();
                }
                if (!ancestor_stack.empty()) {
                    int p = ancestor_stack.back();
                    parent[i] = p;
                    int edge_id = static_cast<int>(edges.size());
                    edges.push_back({p, i, depth[virtual_nodes[i]] - depth[virtual_nodes[p]], 0});
                    virtual_graph[p].push_back({i, edge_id});
                    virtual_graph[i].push_back({p, edge_id});
                }
                ancestor_stack.push_back(i);
            }

            for (int i = 0; i < virtual_size; ++i) {
                subtree_terminals[i] = (color[virtual_nodes[i]] == c);
            }
            for (int i = virtual_size - 1; i >= 0; --i) {
                if (parent[i] != -1) subtree_terminals[parent[i]] += subtree_terminals[i];
            }
            for (VirtualEdge &edge : edges) edge.side_count = subtree_terminals[edge.child];

            Fenwick distribution(m - 1);
            int root = 0;
            int64 distance_sum = 0;
            for (const VirtualEdge &edge : edges) {
                int value = is_ancestor(virtual_nodes[edge.child], virtual_nodes[root])
                                ? m - edge.side_count
                                : edge.side_count;
                distribution.add(value, edge.length);
                distance_sum += static_cast<int64>(edge.length) * value;
            }

            int64 wanted_edges = k[c] - 1LL;
            int64 best = distance_sum - distribution.largest_sum(wanted_edges);

            vector<Event> events;
            events.reserve(3 * virtual_size);
            events.push_back({0, root, -1, 0});

            while (!events.empty()) {
                Event event = events.back();
                events.pop_back();

                if (event.type == 0) {
                    best = min(best, distance_sum - distribution.largest_sum(wanted_edges));
                    const auto &adjacent = virtual_graph[event.node];
                    for (int i = static_cast<int>(adjacent.size()) - 1; i >= 0; --i) {
                        if (adjacent[i].id == event.edge) continue;
                        events.push_back({1, event.node, adjacent[i].id, 0});
                    }
                } else if (event.type == 1) {
                    const VirtualEdge &edge = edges[event.edge];
                    int old_value = (event.node == edge.parent)
                                         ? edge.side_count
                                         : m - edge.side_count;
                    int new_value = m - old_value;

                    distribution.add(old_value, -edge.length);
                    distribution.add(new_value, edge.length);
                    distance_sum += static_cast<int64>(edge.length) * (m - 2LL * old_value);

                    int other = (event.node == edge.parent) ? edge.child : edge.parent;
                    events.push_back({2, event.node, event.edge, old_value});
                    events.push_back({0, other, event.edge, 0});
                } else {
                    const VirtualEdge &edge = edges[event.edge];
                    int new_value = m - event.old_value;
                    distribution.add(new_value, -edge.length);
                    distribution.add(event.old_value, edge.length);
                    distance_sum -= static_cast<int64>(edge.length) *
                                    (m - 2LL * event.old_value);
                }
            }

            answer[c] = best;
        }

        for (int c = 1; c <= n; ++c) {
            if (c > 1) cout << ' ';
            cout << answer[c];
        }
        cout << '\n';
    }

    return 0;
}
