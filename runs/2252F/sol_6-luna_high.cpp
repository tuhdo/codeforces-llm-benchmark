#include <bits/stdc++.h>
using namespace std;

struct Solver {
    int n, LOG;
    vector<vector<int>> graph;
    vector<int> tin, tout, depth, parent;
    vector<vector<int>> up;
    int timer = 0;

    void buildRootedTree() {
        vector<int> nextEdge(n, 0), st = {0};
        parent[0] = 0;
        tin[0] = ++timer;
        up[0][0] = 0;
        for (int j = 1; j < LOG; ++j) up[j][0] = 0;
        while (!st.empty()) {
            int v = st.back();
            if (nextEdge[v] == (int)graph[v].size()) {
                tout[v] = timer;
                st.pop_back();
                continue;
            }
            int to = graph[v][nextEdge[v]++];
            if (to == parent[v]) continue;
            parent[to] = v;
            depth[to] = depth[v] + 1;
            tin[to] = ++timer;
            up[0][to] = v;
            for (int j = 1; j < LOG; ++j) up[j][to] = up[j - 1][up[j - 1][to]];
            st.push_back(to);
        }
    }

    bool ancestor(int a, int b) const {
        return tin[a] <= tin[b] && tout[b] <= tout[a];
    }

    int lca(int a, int b) const {
        if (ancestor(a, b)) return a;
        if (ancestor(b, a)) return b;
        for (int j = LOG - 1; j >= 0; --j) {
            if (!ancestor(up[j][a], b)) a = up[j][a];
        }
        return parent[a];
    }

    long long distance(int a, int b) const {
        int c = lca(a, b);
        return (long long)depth[a] + depth[b] - 2LL * depth[c];
    }

    void run() {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);

        int tests;
        cin >> tests;
        while (tests--) {
            cin >> n;
            vector<int> color(n), k(n);
            vector<vector<int>> byColor(n);
            for (int i = 0; i < n; ++i) {
                cin >> color[i];
                --color[i];
                byColor[color[i]].push_back(i);
            }
            for (int &x : k) cin >> x;

            graph.assign(n, {});
            for (int i = 0; i + 1 < n; ++i) {
                int u, v;
                cin >> u >> v;
                --u; --v;
                graph[u].push_back(v);
                graph[v].push_back(u);
            }

            LOG = 1;
            while ((1 << LOG) <= n) ++LOG;
            tin.assign(n, 0);
            tout.assign(n, 0);
            depth.assign(n, 0);
            parent.assign(n, 0);
            up.assign(LOG, vector<int>(n, 0));
            timer = 0;
            buildRootedTree();

            vector<long long> answer(n, -1);
            for (int c = 0; c < n; ++c) {
                const auto &terminals = byColor[c];
                if (terminals.empty()) continue;
                int m = (int)terminals.size();
                if (m == 1) {
                    answer[c] = 0;
                    continue;
                }
                vector<int> nodes = terminals;
                sort(nodes.begin(), nodes.end(), [&](int a, int b) { return tin[a] < tin[b]; });
                int baseSize = (int)nodes.size();
                for (int i = 1; i < baseSize; ++i) nodes.push_back(lca(nodes[i - 1], nodes[i]));
                sort(nodes.begin(), nodes.end(), [&](int a, int b) { return tin[a] < tin[b]; });
                nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());

                int sz = (int)nodes.size();
                vector<int> par(sz, -1), cnt(sz, 0);
                vector<vector<int>> children(sz);
                vector<int> st;
                for (int i = 0; i < sz; ++i) {
                    while (!st.empty() && !ancestor(nodes[st.back()], nodes[i])) st.pop_back();
                    if (!st.empty()) {
                        par[i] = st.back();
                        children[st.back()].push_back(i);
                    }
                    st.push_back(i);
                    if (color[nodes[i]] == c) cnt[i] = 1;
                }
                for (int i = sz - 1; i > 0; --i) cnt[par[i]] += cnt[i];

                int medianIndex = 0;
                for (int i = 0; i < sz; ++i) {
                    int largest = (par[i] == -1 ? 0 : m - cnt[i]);
                    for (int child : children[i]) largest = max(largest, cnt[child]);
                    if (largest * 2 <= m) {
                        medianIndex = i;
                        break;
                    }
                }
                int median = nodes[medianIndex];

                long long totalDistance = 0;
                for (int v : terminals) totalDistance += distance(median, v);

                vector<int> histogram(m + 1, 0);
                for (int i = 1; i < sz; ++i) {
                    int child = i;
                    int side = ancestor(nodes[child], median) ? m - cnt[child] : cnt[child];
                    if (side > 0) {
                        int len = depth[nodes[child]] - depth[nodes[par[child]]];
                        histogram[side] += len;
                    }
                }

                int need = k[c] - 1;
                long long saved = 0;
                for (int w = m; w >= 1 && need > 0; --w) {
                    int take = min(need, histogram[w]);
                    saved += 1LL * take * w;
                    need -= take;
                }
                answer[c] = totalDistance - saved;
            }

            for (int i = 0; i < n; ++i) {
                if (i) cout << ' ';
                cout << answer[i];
            }
            cout << '\n';
        }
    }
};

int main() {
    Solver solver;
    solver.run();
    return 0;
}
