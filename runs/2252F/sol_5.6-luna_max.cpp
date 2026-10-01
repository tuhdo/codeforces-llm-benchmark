#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

struct Solver {
    int n = 0;
    int log_n = 0;
    int timer = 0;
    vector<vector<int>> graph;
    vector<vector<int>> up;
    vector<int> depth, tin, tout, color;
    vector<vector<int>> by_color;
    vector<int> target;

    bool ancestor(int u, int v) const {
        return tin[u] <= tin[v] && tout[v] <= tout[u];
    }

    int lca(int u, int v) const {
        if (ancestor(u, v)) return u;
        if (ancestor(v, u)) return v;
        for (int j = log_n - 1; j >= 0; --j) {
            if (!ancestor(up[j][u], v)) u = up[j][u];
        }
        return up[0][u];
    }

    int distance(int u, int v) const {
        int w = lca(u, v);
        return depth[u] + depth[v] - 2 * depth[w];
    }

    void preprocess() {
        depth.assign(n + 1, 0);
        tin.assign(n + 1, 0);
        tout.assign(n + 1, 0);
        vector<int> parent(n + 1, 1);

        struct Frame {
            int vertex;
            int parent;
            int next_edge;
        };

        timer = 0;
        vector<Frame> stack;
        stack.push_back({1, 1, 0});
        tin[1] = timer++;

        while (!stack.empty()) {
            Frame &frame = stack.back();
            if (frame.next_edge == static_cast<int>(graph[frame.vertex].size())) {
                tout[frame.vertex] = timer;
                stack.pop_back();
                continue;
            }

            int to = graph[frame.vertex][frame.next_edge++];
            if (to == frame.parent) continue;
            parent[to] = frame.vertex;
            depth[to] = depth[frame.vertex] + 1;
            tin[to] = timer++;
            stack.push_back({to, frame.vertex, 0});
        }

        log_n = 1;
        while ((1 << log_n) <= n) ++log_n;
        up.assign(log_n, vector<int>(n + 1));
        up[0] = parent;
        for (int j = 1; j < log_n; ++j) {
            for (int v = 1; v <= n; ++v) {
                up[j][v] = up[j - 1][up[j - 1][v]];
            }
        }
    }

    vector<int64> solve_colors() {
        vector<int64> answer(n + 1, -1);

        for (int c = 1; c <= n; ++c) {
            auto &marked = by_color[c];
            if (marked.empty()) continue;

            sort(marked.begin(), marked.end(), [&](int u, int v) {
                return tin[u] < tin[v];
            });

            vector<int> vertices = marked;
            vertices.reserve(2 * marked.size());
            for (int i = 1; i < static_cast<int>(marked.size()); ++i) {
                vertices.push_back(lca(marked[i - 1], marked[i]));
            }
            sort(vertices.begin(), vertices.end(), [&](int u, int v) {
                return tin[u] < tin[v];
            });
            vertices.erase(unique(vertices.begin(), vertices.end()), vertices.end());

            int size = static_cast<int>(vertices.size());
            vector<vector<pair<int, int>>> children(size);
            vector<int> virtual_parent(size, -1);
            vector<int> stack;

            for (int i = 0; i < size; ++i) {
                while (!stack.empty() && !ancestor(vertices[stack.back()], vertices[i])) {
                    stack.pop_back();
                }
                if (!stack.empty()) {
                    int p = stack.back();
                    virtual_parent[i] = p;
                    children[p].push_back({i, depth[vertices[i]] - depth[vertices[p]]});
                }
                stack.push_back(i);
            }

            int marked_count = static_cast<int>(marked.size());
            vector<int> subtree_count(size, 0);
            for (int i = 0; i < size; ++i) {
                subtree_count[i] = (color[vertices[i]] == c);
            }
            for (int i = size - 1; i > 0; --i) {
                subtree_count[virtual_parent[i]] += subtree_count[i];
            }

            int median = 0;
            for (int i = 0; i < size; ++i) {
                int largest_part = marked_count - subtree_count[i];
                for (auto [child, length] : children[i]) {
                    largest_part = max(largest_part, subtree_count[child]);
                }
                if (2 * largest_part <= marked_count) {
                    median = i;
                    break;
                }
            }

            int center = vertices[median];
            int64 base_cost = 0;
            for (int vertex : marked) {
                base_cost += distance(center, vertex);
            }

            vector<pair<int, int>> edge_values;
            edge_values.reserve(size - 1);
            for (int child = 1; child < size; ++child) {
                int p = virtual_parent[child];
                int count_on_far_side;
                if (ancestor(vertices[child], center)) {
                    count_on_far_side = marked_count - subtree_count[child];
                } else {
                    count_on_far_side = subtree_count[child];
                }
                int length = depth[vertices[child]] - depth[vertices[p]];
                edge_values.push_back({count_on_far_side, length});
            }

            sort(edge_values.begin(), edge_values.end(), greater<pair<int, int>>());
            int64 remaining = target[c] - 1LL;
            int64 saved = 0;
            for (auto [count, length] : edge_values) {
                if (remaining == 0) break;
                int64 take = min<int64>(remaining, length);
                saved += take * count;
                remaining -= take;
            }

            answer[c] = base_cost - saved;
        }

        return answer;
    }

    void run() {
        cin >> n;
        color.assign(n + 1, 0);
        by_color.assign(n + 1, {});
        for (int v = 1; v <= n; ++v) {
            cin >> color[v];
            by_color[color[v]].push_back(v);
        }

        target.assign(n + 1, 0);
        for (int c = 1; c <= n; ++c) cin >> target[c];

        graph.assign(n + 1, {});
        for (int i = 0; i + 1 < n; ++i) {
            int u, v;
            cin >> u >> v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }

        preprocess();
        vector<int64> answer = solve_colors();
        for (int c = 1; c <= n; ++c) {
            if (c > 1) cout << ' ';
            cout << answer[c];
        }
        cout << '\n';
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        Solver solver;
        solver.run();
    }
    return 0;
}
