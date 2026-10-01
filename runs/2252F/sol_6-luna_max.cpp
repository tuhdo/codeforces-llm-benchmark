#include <bits/stdc++.h>
using namespace std;

static constexpr int LOG = 20;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCases;
    cin >> testCases;
    while (testCases--) {
        int n;
        cin >> n;

        vector<int> color(n), targetSize(n);
        vector<vector<int>> verticesByColor(n);
        for (int v = 0; v < n; ++v) {
            cin >> color[v];
            --color[v];
            verticesByColor[color[v]].push_back(v);
        }
        for (int &k : targetSize) cin >> k;

        vector<vector<int>> graph(n);
        for (int i = 0; i < n - 1; ++i) {
            int u, v;
            cin >> u >> v;
            --u;
            --v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }

        vector<int> depth(n), tin(n), tout(n), parent(n);
        vector<array<int, LOG>> up(n);

        int timer = 0;
        parent[0] = 0;
        tin[0] = timer++;
        up[0].fill(0);
        vector<pair<int, int>> dfsStack;
        dfsStack.emplace_back(0, 0);
        while (!dfsStack.empty()) {
            int v = dfsStack.back().first;
            int &nextEdge = dfsStack.back().second;

            if (nextEdge < static_cast<int>(graph[v].size())) {
                int to = graph[v][nextEdge++];
                if (to == parent[v]) continue;

                parent[to] = v;
                depth[to] = depth[v] + 1;
                tin[to] = timer++;
                up[to][0] = v;
                for (int j = 1; j < LOG; ++j) {
                    up[to][j] = up[up[to][j - 1]][j - 1];
                }
                dfsStack.emplace_back(to, 0);
            } else {
                tout[v] = timer++;
                dfsStack.pop_back();
            }
        }

        auto isAncestor = [&](int a, int b) {
            return tin[a] <= tin[b] && tout[a] >= tout[b];
        };

        auto lca = [&](int a, int b) {
            if (isAncestor(a, b)) return a;
            if (isAncestor(b, a)) return b;
            for (int j = LOG - 1; j >= 0; --j) {
                if (!isAncestor(up[a][j], b)) a = up[a][j];
            }
            return parent[a];
        };

        vector<long long> answer(n, -1);
        for (int c = 0; c < n; ++c) {
            const auto &coloredVertices = verticesByColor[c];
            if (coloredVertices.empty()) continue;

            const int m = static_cast<int>(coloredVertices.size());
            vector<int> sortedTerminals = coloredVertices;
            sort(sortedTerminals.begin(), sortedTerminals.end(),
                 [&](int a, int b) { return tin[a] < tin[b]; });

            vector<int> virtualNodes = sortedTerminals;
            for (int i = 0; i + 1 < m; ++i) {
                virtualNodes.push_back(lca(sortedTerminals[i], sortedTerminals[i + 1]));
            }
            sort(virtualNodes.begin(), virtualNodes.end(),
                 [&](int a, int b) { return tin[a] < tin[b]; });
            virtualNodes.erase(unique(virtualNodes.begin(), virtualNodes.end()),
                               virtualNodes.end());

            const int s = static_cast<int>(virtualNodes.size());
            vector<int> virtualParent(s, -1), subtreeCount(s, 0), stack;
            stack.reserve(s);
            for (int i = 0; i < s; ++i) {
                int v = virtualNodes[i];
                if (color[v] == c) subtreeCount[i] = 1;

                while (!stack.empty() && !isAncestor(virtualNodes[stack.back()], v)) {
                    stack.pop_back();
                }
                if (!stack.empty()) virtualParent[i] = stack.back();
                stack.push_back(i);
            }

            for (int i = s - 1; i >= 0; --i) {
                if (virtualParent[i] != -1) {
                    subtreeCount[virtualParent[i]] += subtreeCount[i];
                }
            }

            long long singletonCost = 0;
            vector<pair<int, int>> edgeWeights;
            edgeWeights.reserve(s);
            for (int i = 0; i < s; ++i) {
                int p = virtualParent[i];
                if (p == -1) continue;

                int sideCount = subtreeCount[i];
                int weight = min(sideCount, m - sideCount);
                if (weight == 0) continue;

                int length = depth[virtualNodes[i]] - depth[virtualNodes[p]];
                singletonCost += 1LL * weight * length;
                edgeWeights.emplace_back(weight, length);
            }

            sort(edgeWeights.begin(), edgeWeights.end(),
                 [](const auto &a, const auto &b) { return a.first > b.first; });

            int edgesToTake = targetSize[c] - 1;
            long long saved = 0;
            for (auto [weight, length] : edgeWeights) {
                int take = min(edgesToTake, length);
                saved += 1LL * weight * take;
                edgesToTake -= take;
                if (edgesToTake == 0) break;
            }

            answer[c] = singletonCost - saved;
        }

        for (int c = 0; c < n; ++c) {
            if (c) cout << ' ';
            cout << answer[c];
        }
        cout << '\n';
    }

    return 0;
}
