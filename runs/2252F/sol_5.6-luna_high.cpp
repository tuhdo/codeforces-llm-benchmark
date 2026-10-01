#include <bits/stdc++.h>
using namespace std;

using ll = long long;

struct Fenwick {
    int n;
    vector<ll> cnt, sum;

    explicit Fenwick(int n = 0) { init(n); }

    void init(int n_) {
        n = n_;
        cnt.assign(n + 1, 0);
        sum.assign(n + 1, 0);
    }

    void add(int pos, ll dc, ll ds) {
        for (int i = pos; i <= n; i += i & -i) {
            cnt[i] += dc;
            sum[i] += ds;
        }
    }

    // Sum of the values of the k smallest stored items.
    ll sumSmallest(ll k) const {
        if (k <= 0) return 0;

        int pos = 0;
        ll used = 0;
        ll result = 0;
        int step = 1;
        while ((step << 1) <= n) step <<= 1;

        for (; step; step >>= 1) {
            int nxt = pos + step;
            if (nxt <= n && used + cnt[nxt] < k) {
                pos = nxt;
                used += cnt[nxt];
                result += sum[nxt];
            }
        }

        int value = pos + 1;
        result += (k - used) * value;
        return result;
    }
};

struct VirtualEdge {
    int to;
    int len;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;

        vector<int> color(n + 1);
        vector<vector<int>> byColor(n + 1);
        for (int v = 1; v <= n; ++v) {
            cin >> color[v];
            byColor[color[v]].push_back(v);
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

        int LOG = 1;
        while ((1 << LOG) <= n) ++LOG;

        vector<int> depth(n + 1), tin(n + 1), tout(n + 1);
        vector<vector<int>> up(LOG, vector<int>(n + 1));

        int timer = 0;
        vector<int> iteratorIndex(n + 1, 0);
        vector<int> dfs;
        dfs.reserve(n);
        dfs.push_back(1);
        tin[1] = timer++;
        up[0][1] = 0;

        while (!dfs.empty()) {
            int v = dfs.back();
            if (iteratorIndex[v] == (int)graph[v].size()) {
                tout[v] = timer - 1;
                dfs.pop_back();
                continue;
            }

            int to = graph[v][iteratorIndex[v]++];
            if (to == up[0][v]) continue;
            up[0][to] = v;
            depth[to] = depth[v] + 1;
            tin[to] = timer++;
            dfs.push_back(to);
        }

        for (int j = 1; j < LOG; ++j) {
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
            int v = a;
            for (int j = LOG - 1; j >= 0; --j) {
                if (up[j][v] != 0 && !isAncestor(up[j][v], b)) {
                    v = up[j][v];
                }
            }
            return up[0][v];
        };

        auto distance = [&](int u, int v) -> int {
            int w = lca(u, v);
            return depth[u] + depth[v] - 2 * depth[w];
        };

        vector<ll> answer(n + 1, -1);
        vector<int> localId(n + 1, -1);

        for (int c = 1; c <= n; ++c) {
            const vector<int>& terminals = byColor[c];
            int m = (int)terminals.size();
            if (m == 0) continue;

            vector<int> sortedTerminals = terminals;
            sort(sortedTerminals.begin(), sortedTerminals.end(), [&](int a, int b) {
                return tin[a] < tin[b];
            });
            vector<int> nodes = sortedTerminals;
            for (int i = 1; i < m; ++i) nodes.push_back(lca(sortedTerminals[i - 1], sortedTerminals[i]));
            sort(nodes.begin(), nodes.end(), [&](int a, int b) {
                return tin[a] < tin[b];
            });
            nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());

            int sz = (int)nodes.size();
            for (int i = 0; i < sz; ++i) localId[nodes[i]] = i;

            vector<vector<VirtualEdge>> tree(sz);
            vector<int> parent(sz, -1), edgeLen(sz, 0);
            vector<int> st;
            st.reserve(sz);

            for (int i = 0; i < sz; ++i) {
                while (!st.empty() && !isAncestor(nodes[st.back()], nodes[i])) st.pop_back();
                if (!st.empty()) {
                    int p = st.back();
                    int len = distance(nodes[p], nodes[i]);
                    parent[i] = p;
                    edgeLen[i] = len;
                    tree[p].push_back({i, len});
                    tree[i].push_back({p, len});
                }
                st.push_back(i);
            }

            vector<ll> sub(sz, 0);
            for (int v : terminals) ++sub[localId[v]];
            for (int i = sz - 1; i > 0; --i) sub[parent[i]] += sub[i];

            ll baseline = 0;
            Fenwick fenwick(m);
            ll totalCount = 0;
            ll totalSum = 0;

            auto changeGroup = [&](int value, ll amount) {
                if (value <= 0 || amount == 0) return;
                fenwick.add(value, amount, amount * value);
                totalCount += amount;
                totalSum += amount * value;
            };

            for (int v = 1; v < sz; ++v) {
                baseline += sub[v] * edgeLen[v];
                changeGroup((int)sub[v], edgeLen[v]);
            }

            ll q = k[c] - 1LL;
            ll best = numeric_limits<ll>::max();

            auto currentSaving = [&]() -> ll {
                if (q == 0) return 0;
                return totalSum - fenwick.sumSmallest(totalCount - q);
            };

            struct Event {
                int v;
                int p;
                bool exit;
            };

            vector<Event> events;
            events.reserve(2 * sz);
            events.push_back({0, -1, false});
            while (!events.empty()) {
                Event event = events.back();
                events.pop_back();

                int v = event.v;
                int p = event.p;
                if (event.exit) {
                    int s = (int)sub[v];
                    changeGroup(m - s, -edgeLen[v]);
                    changeGroup(s, edgeLen[v]);
                    baseline -= (ll)(m - 2 * s) * edgeLen[v];
                    continue;
                }

                if (p != -1) {
                    int s = (int)sub[v];
                    changeGroup(s, -edgeLen[v]);
                    changeGroup(m - s, edgeLen[v]);
                    baseline += (ll)(m - 2 * s) * edgeLen[v];
                }

                best = min(best, baseline - currentSaving());

                for (int i = (int)tree[v].size() - 1; i >= 0; --i) {
                    int to = tree[v][i].to;
                    if (to == p) continue;
                    events.push_back({to, v, true});
                    events.push_back({to, v, false});
                }
            }

            answer[c] = best;

            for (int v : nodes) localId[v] = -1;
        }

        for (int c = 1; c <= n; ++c) {
            if (c > 1) cout << ' ';
            cout << answer[c];
        }
        cout << '\n';
    }
    return 0;
}
