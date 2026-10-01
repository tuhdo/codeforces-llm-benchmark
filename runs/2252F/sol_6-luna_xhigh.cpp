#include <bits/stdc++.h>
using namespace std;

struct EdgeInfo {
    int count;
    int length;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;

        vector<int> color(n + 1), target(n + 1);
        vector<vector<int>> groups(n + 1);
        for (int v = 1; v <= n; ++v) {
            cin >> color[v];
            groups[color[v]].push_back(v);
        }
        for (int c = 1; c <= n; ++c) cin >> target[c];

        vector<vector<int>> graph(n + 1);
        for (int i = 1; i < n; ++i) {
            int u, v;
            cin >> u >> v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }

        int logn = 1;
        while ((1 << logn) <= n) ++logn;
        vector<vector<int>> up(logn, vector<int>(n + 1));
        vector<int> depth(n + 1), tin(n + 1), tout(n + 1), parent(n + 1);
        vector<int> order;
        order.reserve(n);

        vector<int> stack{1};
        parent[1] = 0;
        while (!stack.empty()) {
            int u = stack.back();
            stack.pop_back();
            tin[u] = (int)order.size();
            order.push_back(u);
            for (int v : graph[u]) {
                if (v == parent[u]) continue;
                parent[v] = u;
                depth[v] = depth[u] + 1;
                stack.push_back(v);
            }
        }
        vector<int> subtreeSize(n + 1, 1);
        for (int i = n - 1; i > 0; --i) {
            int u = order[i];
            subtreeSize[parent[u]] += subtreeSize[u];
        }
        for (int u = 1; u <= n; ++u) tout[u] = tin[u] + subtreeSize[u];

        for (int v = 1; v <= n; ++v) up[0][v] = parent[v];
        for (int j = 1; j < logn; ++j) {
            for (int v = 1; v <= n; ++v) {
                up[j][v] = up[j - 1][up[j - 1][v]];
            }
        }

        auto isAncestor = [&](int u, int v) {
            return tin[u] <= tin[v] && tout[v] <= tout[u];
        };
        auto lca = [&](int a, int b) {
            if (isAncestor(a, b)) return a;
            if (isAncestor(b, a)) return b;
            int x = a;
            for (int j = logn - 1; j >= 0; --j) {
                if (up[j][x] && !isAncestor(up[j][x], b)) x = up[j][x];
            }
            return parent[x];
        };
        auto distance = [&](int a, int b) {
            int w = lca(a, b);
            return depth[a] + depth[b] - 2 * depth[w];
        };

        vector<long long> answer(n + 1, -1);
        vector<int> localIndex(n + 1, -1);

        for (int c = 1; c <= n; ++c) {
            const auto &terminals = groups[c];
            int m = (int)terminals.size();
            if (m == 0) continue;
            if (m == 1) {
                answer[c] = 0;
                continue;
            }

            vector<int> nodes = terminals;
            sort(nodes.begin(), nodes.end(), [&](int a, int b) { return tin[a] < tin[b]; });
            for (int i = 1; i < m; ++i) nodes.push_back(lca(nodes[i - 1], nodes[i]));
            sort(nodes.begin(), nodes.end(), [&](int a, int b) { return tin[a] < tin[b]; });
            nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());

            int s = (int)nodes.size();
            for (int i = 0; i < s; ++i) localIndex[nodes[i]] = i;

            vector<int> virtualParent(s, -1), virtualStack;
            virtualStack.reserve(s);
            for (int i = 0; i < s; ++i) {
                while (!virtualStack.empty() && !isAncestor(nodes[virtualStack.back()], nodes[i])) {
                    virtualStack.pop_back();
                }
                if (!virtualStack.empty()) virtualParent[i] = virtualStack.back();
                virtualStack.push_back(i);
            }

            vector<int> weight(s), subtreeWeight(s);
            for (int v : terminals) weight[localIndex[v]] = 1;
            subtreeWeight = weight;
            vector<vector<int>> adjacent(s);
            for (int i = 0; i < s; ++i) {
                int p = virtualParent[i];
                if (p != -1) {
                    adjacent[p].push_back(i);
                    adjacent[i].push_back(p);
                }
            }
            for (int i = s - 1; i > 0; --i) {
                subtreeWeight[virtualParent[i]] += subtreeWeight[i];
            }

            int median = 0;
            for (int i = 0; i < s; ++i) {
                int largestSide = m - subtreeWeight[i];
                for (int child : adjacent[i]) {
                    if (virtualParent[child] == i) largestSide = max(largestSide, subtreeWeight[child]);
                }
                if (largestSide * 2 <= m) {
                    median = i;
                    break;
                }
            }

            vector<EdgeInfo> edgeInfos;
            edgeInfos.reserve(s - 1);
            vector<int> dfsParent(s, -2), dfsOrder;
            dfsParent[median] = -1;
            dfsOrder.push_back(median);
            for (int at = 0; at < (int)dfsOrder.size(); ++at) {
                int u = dfsOrder[at];
                for (int v : adjacent[u]) {
                    if (v == dfsParent[u]) continue;
                    dfsParent[v] = u;
                    int sideCount = (virtualParent[v] == u) ? subtreeWeight[v] : m - subtreeWeight[u];
                    int len = abs(depth[nodes[u]] - depth[nodes[v]]);
                    edgeInfos.push_back({sideCount, len});
                    dfsOrder.push_back(v);
                }
            }

            sort(edgeInfos.begin(), edgeInfos.end(), [](const EdgeInfo &a, const EdgeInfo &b) {
                return a.count > b.count;
            });
            long long improvement = 0;
            int need = target[c] - 1;
            for (const auto &e : edgeInfos) {
                if (need == 0) break;
                int take = min(need, e.length);
                improvement += 1LL * take * e.count;
                need -= take;
            }

            long long baseCost = 0;
            int medianVertex = nodes[median];
            for (int v : terminals) baseCost += distance(medianVertex, v);
            answer[c] = baseCost - improvement;

            for (int v : nodes) localIndex[v] = -1;
        }

        for (int c = 1; c <= n; ++c) {
            if (c > 1) cout << ' ';
            cout << answer[c];
        }
        cout << '\n';
    }
    return 0;
}
