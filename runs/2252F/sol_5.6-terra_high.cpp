#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

using namespace std;

class Solver {
    static constexpr int LOG = 19;

    int n = 0;
    vector<vector<int>> graph;
    vector<int> tin, tout, depth;
    vector<array<int, LOG>> up;
    int timer = 0;

    bool isAncestor(int u, int v) const {
        return tin[u] <= tin[v] && tout[v] <= tout[u];
    }

    int lca(int u, int v) const {
        if (isAncestor(u, v)) return u;
        if (isAncestor(v, u)) return v;
        for (int j = LOG - 1; j >= 0; --j) {
            if (!isAncestor(up[u][j], v)) u = up[u][j];
        }
        return up[u][0];
    }

    void buildLca() {
        tin.assign(n + 1, 0);
        tout.assign(n + 1, 0);
        depth.assign(n + 1, 0);
        up.assign(n + 1, {});
        timer = 0;

        struct Frame {
            int vertex;
            int parent;
            int nextEdge;
        };

        vector<Frame> stack;
        stack.push_back({1, 1, 0});
        tin[1] = ++timer;
        up[1].fill(1);

        while (!stack.empty()) {
            Frame& frame = stack.back();
            if (frame.nextEdge == static_cast<int>(graph[frame.vertex].size())) {
                tout[frame.vertex] = ++timer;
                stack.pop_back();
                continue;
            }

            int to = graph[frame.vertex][frame.nextEdge++];
            if (to == frame.parent) continue;

            depth[to] = depth[frame.vertex] + 1;
            up[to][0] = frame.vertex;
            for (int j = 1; j < LOG; ++j) {
                up[to][j] = up[up[to][j - 1]][j - 1];
            }
            tin[to] = ++timer;
            stack.push_back({to, frame.vertex, 0});
        }
    }

public:
    void solve() {
        cin >> n;
        vector<vector<int>> byColor(n + 1);
        for (int vertex = 1; vertex <= n; ++vertex) {
            int color;
            cin >> color;
            byColor[color].push_back(vertex);
        }

        vector<int> targetSize(n + 1);
        for (int color = 1; color <= n; ++color) cin >> targetSize[color];

        graph.assign(n + 1, {});
        for (int i = 1; i < n; ++i) {
            int u, v;
            cin >> u >> v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }
        buildLca();

        vector<long long> answer(n + 1, -1);
        vector<int> markedCount(n + 1, 0);

        for (int color = 1; color <= n; ++color) {
            const vector<int>& marked = byColor[color];
            if (marked.empty()) continue;

            vector<int> vertices = marked;
            sort(vertices.begin(), vertices.end(), [this](int a, int b) {
                return tin[a] < tin[b];
            });

            const int markedSize = static_cast<int>(vertices.size());
            for (int i = 1; i < markedSize; ++i) {
                vertices.push_back(lca(vertices[i - 1], vertices[i]));
            }
            sort(vertices.begin(), vertices.end(), [this](int a, int b) {
                return tin[a] < tin[b];
            });
            vertices.erase(unique(vertices.begin(), vertices.end()), vertices.end());

            for (int vertex : marked) markedCount[vertex] = 1;

            vector<pair<int, int>> virtualEdges;
            vector<int> stack;
            for (int vertex : vertices) {
                while (!stack.empty() && !isAncestor(stack.back(), vertex)) {
                    stack.pop_back();
                }
                if (!stack.empty()) virtualEdges.push_back({stack.back(), vertex});
                stack.push_back(vertex);
            }

            long long baseCost = 0;
            vector<pair<int, int>> weightedPaths;
            for (int i = static_cast<int>(virtualEdges.size()) - 1; i >= 0; --i) {
                const auto [parent, child] = virtualEdges[i];
                const int inside = markedCount[child];
                markedCount[parent] += inside;

                const int weight = min(inside, markedSize - inside);
                const int length = depth[child] - depth[parent];
                baseCost += 1LL * weight * length;
                weightedPaths.push_back({weight, length});
            }

            sort(weightedPaths.rbegin(), weightedPaths.rend());
            int edgesToKeep = targetSize[color] - 1;
            long long saved = 0;
            for (const auto [weight, length] : weightedPaths) {
                const int taken = min(edgesToKeep, length);
                saved += 1LL * taken * weight;
                edgesToKeep -= taken;
                if (edgesToKeep == 0) break;
            }
            answer[color] = baseCost - saved;

            for (int vertex : vertices) markedCount[vertex] = 0;
        }

        for (int color = 1; color <= n; ++color) {
            cout << answer[color] << " \n"[color == n];
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCases;
    cin >> testCases;
    while (testCases--) {
        Solver solver;
        solver.solve();
    }
}
