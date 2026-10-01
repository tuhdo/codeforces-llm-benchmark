#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

struct Fenwick {
    int n;
    vector<int64> count, sum;

    explicit Fenwick(int n = 0) : n(n), count(n + 1), sum(n + 1) {}

    void add(int pos, int64 amount) {
        for (int i = pos; i <= n; i += i & -i) {
            count[i] += amount;
            sum[i] += amount * pos;
        }
    }

    int64 prefixCount(int pos) const {
        int64 result = 0;
        for (int i = pos; i > 0; i -= i & -i) result += count[i];
        return result;
    }

    int64 prefixSum(int pos) const {
        int64 result = 0;
        for (int i = pos; i > 0; i -= i & -i) result += sum[i];
        return result;
    }

    int64 totalCount() const { return prefixCount(n); }
    int64 totalSum() const { return prefixSum(n); }

    // Sum of the smallest 'take' elements in the multiset.
    int64 smallestSum(int64 take) const {
        if (take == 0) return 0;

        int pos = 0;
        int64 before = 0;
        for (int step = 1 << (31 - __builtin_clz(n)); step; step >>= 1) {
            int next = pos + step;
            if (next <= n && before + count[next] < take) {
                pos = next;
                before += count[next];
            }
        }
        return prefixSum(pos) + (take - before) * (pos + 1);
    }

    int64 largestSum(int64 take) const {
        return totalSum() - smallestSum(totalCount() - take);
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

        vector<int> color(n + 1), need(n + 1);
        vector<vector<int>> byColor(n + 1);
        for (int v = 1; v <= n; ++v) {
            cin >> color[v];
            byColor[color[v]].push_back(v);
        }
        for (int c = 1; c <= n; ++c) cin >> need[c];

        vector<vector<int>> graph(n + 1);
        for (int i = 1; i < n; ++i) {
            int u, v;
            cin >> u >> v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }

        constexpr int LOG = 19;
        vector<array<int, LOG>> up(n + 1);
        vector<int> depth(n + 1), tin(n + 1), tout(n + 1), parent(n + 1);
        int timer = 0;

        struct DfsEvent { int v, p, state; };
        vector<DfsEvent> dfs;
        dfs.reserve(2 * n);
        dfs.push_back({1, 1, 0});
        while (!dfs.empty()) {
            auto [v, p, state] = dfs.back();
            dfs.pop_back();
            if (state == 0) {
                parent[v] = p;
                up[v][0] = p;
                for (int j = 1; j < LOG; ++j) up[v][j] = up[up[v][j - 1]][j - 1];
                tin[v] = timer++;
                dfs.push_back({v, p, 1});
                for (int i = (int)graph[v].size() - 1; i >= 0; --i) {
                    int to = graph[v][i];
                    if (to == p) continue;
                    depth[to] = depth[v] + 1;
                    dfs.push_back({to, v, 0});
                }
            } else {
                tout[v] = timer;
            }
        }

        auto isAncestor = [&](int u, int v) {
            return tin[u] <= tin[v] && tout[v] <= tout[u];
        };
        auto lca = [&](int a, int b) {
            if (isAncestor(a, b)) return a;
            if (isAncestor(b, a)) return b;
            for (int j = LOG - 1; j >= 0; --j) {
                if (!isAncestor(up[a][j], b)) a = up[a][j];
            }
            return up[a][0];
        };

        vector<int64> answer(n + 1, -1);
        for (int c = 1; c <= n; ++c) {
            const vector<int>& terminals = byColor[c];
            const int m = (int)terminals.size();
            if (m == 0) continue;

            vector<int> nodes = terminals;
            sort(nodes.begin(), nodes.end(), [&](int a, int b) { return tin[a] < tin[b]; });
            const int originalSize = (int)nodes.size();
            for (int i = 1; i < originalSize; ++i) nodes.push_back(lca(nodes[i - 1], nodes[i]));
            sort(nodes.begin(), nodes.end(), [&](int a, int b) { return tin[a] < tin[b]; });
            nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());

            const int sz = (int)nodes.size();
            vector<vector<pair<int, int>>> tree(sz);
            vector<int> stack;
            stack.reserve(sz);
            for (int i = 0; i < sz; ++i) {
                while (!stack.empty() && !isAncestor(nodes[stack.back()], nodes[i])) stack.pop_back();
                if (!stack.empty()) {
                    int p = stack.back();
                    int length = depth[nodes[i]] - depth[nodes[p]];
                    tree[p].push_back({i, length});
                    tree[i].push_back({p, length});
                }
                stack.push_back(i);
            }

            vector<int> vtParent(sz, -1), edgeLength(sz), order;
            order.reserve(sz);
            vector<int> traversal = {0};
            vtParent[0] = 0;
            while (!traversal.empty()) {
                int v = traversal.back();
                traversal.pop_back();
                order.push_back(v);
                for (auto [to, length] : tree[v]) {
                    if (to == vtParent[v]) continue;
                    vtParent[to] = v;
                    edgeLength[to] = length;
                    traversal.push_back(to);
                }
            }

            vector<int> subtreeTerminals(sz);
            for (int i = 0; i < sz; ++i) subtreeTerminals[i] = (color[nodes[i]] == c);
            for (int i = sz - 1; i > 0; --i) {
                int v = order[i];
                subtreeTerminals[vtParent[v]] += subtreeTerminals[v];
            }

            int64 rootDistanceSum = 0;
            Fenwick values(m);
            for (int v = 1; v < sz; ++v) {
                rootDistanceSum += 1LL * edgeLength[v] * subtreeTerminals[v];
                values.add(subtreeTerminals[v], edgeLength[v]);
            }

            const int componentSize = need[c];
            const int64 selectedExtraVertices = componentSize - 1;
            int64 best = numeric_limits<int64>::max();

            struct RerootEvent {
                int v;
                int64 distanceSum;
                bool leaving;
            };
            vector<RerootEvent> reroot;
            reroot.reserve(2 * sz);
            reroot.push_back({0, rootDistanceSum, false});
            while (!reroot.empty()) {
                RerootEvent event = reroot.back();
                reroot.pop_back();
                int v = event.v;
                if (event.leaving) {
                    values.add(m - subtreeTerminals[v], -edgeLength[v]);
                    values.add(subtreeTerminals[v], edgeLength[v]);
                    continue;
                }

                if (v != 0) {
                    values.add(subtreeTerminals[v], -edgeLength[v]);
                    values.add(m - subtreeTerminals[v], edgeLength[v]);
                }
                best = min(best, event.distanceSum - values.largestSum(selectedExtraVertices));

                if (v != 0) reroot.push_back({v, 0, true});
                for (auto [to, length] : tree[v]) {
                    if (to == vtParent[v]) continue;
                    int64 nextDistance = event.distanceSum + 1LL * length * (m - 2LL * subtreeTerminals[to]);
                    reroot.push_back({to, nextDistance, false});
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
}
