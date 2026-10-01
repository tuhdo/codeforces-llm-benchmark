#include <bits/stdc++.h>
using namespace std;

static const int MOD = 998244353;
static const int G = 3;

int mod_pow(int a, int e) {
    long long r = 1;
    while (e > 0) {
        if (e & 1) r = r * a % MOD;
        a = (int)(1LL * a * a % MOD);
        e >>= 1;
    }
    return (int)r;
}

void ntt(vector<int>& a, bool invert) {
    int n = (int)a.size();
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }

    for (int len = 2; len <= n; len <<= 1) {
        int wlen = mod_pow(G, (MOD - 1) / len);
        if (invert) wlen = mod_pow(wlen, MOD - 2);
        for (int i = 0; i < n; i += len) {
            long long w = 1;
            int half = len >> 1;
            for (int j = 0; j < half; ++j) {
                int u = a[i + j];
                int v = (int)(w * a[i + j + half] % MOD);
                int x = u + v;
                if (x >= MOD) x -= MOD;
                int y = u - v;
                if (y < 0) y += MOD;
                a[i + j] = x;
                a[i + j + half] = y;
                w = w * wlen % MOD;
            }
        }
    }

    if (invert) {
        int inv_n = mod_pow(n, MOD - 2);
        for (int& x : a) x = (int)(1LL * x * inv_n % MOD);
    }
}

vector<int> lca_depths(const vector<vector<int>>& g, int center, int blocked, int h) {
    int n = (int)g.size() - 1;
    vector<int> parent(n + 1, -1), depth(n + 1), order;
    order.reserve(n);
    parent[center] = 0;
    order.push_back(center);

    for (size_t i = 0; i < order.size(); ++i) {
        int v = order[i];
        for (int u : g[v]) {
            if (u == blocked || u == parent[v]) continue;
            parent[u] = v;
            depth[u] = depth[v] + 1;
            order.push_back(u);
        }
    }

    vector<int> marked_count(n + 1, 0);
    vector<int> possible(h + 1, 0);
    possible[h] = 1; // Choosing the same endpoint twice.

    for (int i = (int)order.size() - 1; i >= 0; --i) {
        int v = order[i];
        int child_subtrees = 0;
        int total = (depth[v] == h);
        for (int u : g[v]) {
            if (parent[u] == v) {
                total += marked_count[u];
                if (marked_count[u] > 0) ++child_subtrees;
            }
        }
        marked_count[v] = total;
        if (child_subtrees >= 2) possible[depth[v]] = 1;
    }
    return possible;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        vector<vector<int>> g(n + 1);
        for (int i = 0; i < n - 1; ++i) {
            int u, v;
            cin >> u >> v;
            g[u].push_back(v);
            g[v].push_back(u);
        }

        auto farthest = [&](int start) {
            vector<int> dist(n + 1, -1), par(n + 1, 0), q;
            q.reserve(n);
            q.push_back(start);
            dist[start] = 0;
            for (size_t i = 0; i < q.size(); ++i) {
                int v = q[i];
                for (int u : g[v]) if (dist[u] == -1) {
                    dist[u] = dist[v] + 1;
                    par[u] = v;
                    q.push_back(u);
                }
            }
            int endpoint = start;
            for (int v : q) if (dist[v] > dist[endpoint]) endpoint = v;
            return tuple<int, vector<int>, vector<int>>(endpoint, move(dist), move(par));
        };

        auto [a, ignored_dist, ignored_parent] = farthest(1);
        auto [b, dist_a, parent_bfs] = farthest(a);
        int diameter = dist_a[b];
        int h = diameter / 2;

        vector<int> path;
        for (int v = b; v != 0; v = parent_bfs[v]) {
            path.push_back(v);
            if (v == a) break;
        }
        reverse(path.begin(), path.end());
        int c1 = path[h], c2 = path[h + 1];

        vector<int> left = lca_depths(g, c1, c2, h);
        vector<int> right = lca_depths(g, c2, c1, h);

        int need = 1;
        while (need <= 2 * h) need <<= 1;
        left.resize(need);
        right.resize(need);
        ntt(left, false);
        ntt(right, false);
        for (int i = 0; i < need; ++i) left[i] = (int)(1LL * left[i] * right[i] % MOD);
        ntt(left, true);

        vector<int> answer;
        for (int sum = 0; sum <= 2 * h; ++sum) {
            if (left[sum] != 0) answer.push_back(sum + 1);
        }
        cout << answer.size();
        for (size_t i = 0; i < answer.size(); ++i) {
            cout << ' ';
            cout << answer[i];
        }
        cout << '\n';
    }
}
