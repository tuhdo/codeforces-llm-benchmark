#include <bits/stdc++.h>
using namespace std;

class Solver {
    static constexpr int LOG = 20;

    int n;
    vector<vector<int>> graph;
    vector<int> depth, tin, tout;
    vector<array<int, LOG>> up;
    int timer = 0;

    bool is_ancestor(int a, int b) const {
        return tin[a] <= tin[b] && tout[b] <= tout[a];
    }

    int lca(int a, int b) const {
        if (is_ancestor(a, b)) return a;
        if (is_ancestor(b, a)) return b;
        for (int j = LOG - 1; j >= 0; --j) {
            if (!is_ancestor(up[a][j], b)) a = up[a][j];
        }
        return up[a][0];
    }

    void preprocess() {
        depth.assign(n + 1, 0);
        tin.assign(n + 1, 0);
        tout.assign(n + 1, 0);
        up.assign(n + 1, {});

        vector<int> parent(n + 1, 0);
        parent[1] = 1;
        vector<int> events = {1};
        timer = 0;

        while (!events.empty()) {
            int x = events.back();
            events.pop_back();
            if (x < 0) {
                tout[-x] = timer - 1;
                continue;
            }

            int v = x;
            tin[v] = timer++;
            up[v][0] = parent[v];
            for (int j = 1; j < LOG; ++j) up[v][j] = up[up[v][j - 1]][j - 1];

            events.push_back(-v);
            for (int i = (int)graph[v].size() - 1; i >= 0; --i) {
                int to = graph[v][i];
                if (to == parent[v]) continue;
                parent[to] = v;
                depth[to] = depth[v] + 1;
                events.push_back(to);
            }
        }
    }

public:
    void solve_case() {
        cin >> n;
        vector<vector<int>> by_color(n + 1);
        for (int v = 1; v <= n; ++v) {
            int color;
            cin >> color;
            by_color[color].push_back(v);
        }

        vector<int> want(n + 1);
        for (int color = 1; color <= n; ++color) cin >> want[color];

        graph.assign(n + 1, {});
        for (int i = 1; i < n; ++i) {
            int u, v;
            cin >> u >> v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }
        preprocess();

        vector<long long> answer(n + 1, -1);
        vector<int> terminal_count(n + 1, 0);

        for (int color = 1; color <= n; ++color) {
            const vector<int>& terminals = by_color[color];
            if (terminals.empty()) continue;

            vector<int> nodes = terminals;
            sort(nodes.begin(), nodes.end(), [&](int a, int b) {
                return tin[a] < tin[b];
            });
            int original_size = (int)nodes.size();
            for (int i = 1; i < original_size; ++i) {
                nodes.push_back(lca(nodes[i - 1], nodes[i]));
            }
            sort(nodes.begin(), nodes.end(), [&](int a, int b) {
                return tin[a] < tin[b];
            });
            nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());

            vector<pair<int, int>> edges;
            vector<int> stack;
            for (int v : nodes) {
                while (!stack.empty() && !is_ancestor(stack.back(), v)) stack.pop_back();
                if (!stack.empty()) edges.push_back({stack.back(), v});
                stack.push_back(v);
            }

            for (int v : terminals) terminal_count[v] = 1;

            long long base_cost = 0;
            vector<pair<int, int>> weighted_paths;
            for (int i = (int)edges.size() - 1; i >= 0; --i) {
                auto [parent, child] = edges[i];
                int inside = terminal_count[child];
                terminal_count[parent] += inside;

                int weight = min(inside, (int)terminals.size() - inside);
                int length = depth[child] - depth[parent];
                base_cost += 1LL * weight * length;
                weighted_paths.push_back({weight, length});
            }

            sort(weighted_paths.rbegin(), weighted_paths.rend());
            long long saved = 0;
            int remaining = want[color] - 1;
            for (auto [weight, length] : weighted_paths) {
                int take = min(remaining, length);
                saved += 1LL * take * weight;
                remaining -= take;
                if (remaining == 0) break;
            }
            answer[color] = base_cost - saved;

            for (int v : nodes) terminal_count[v] = 0;
        }

        for (int color = 1; color <= n; ++color) {
            if (color > 1) cout << ' ';
            cout << answer[color];
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
        solver.solve_case();
    }
}
