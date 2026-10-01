#include <bits/stdc++.h>
using namespace std;

static constexpr int MOD = 998244353;
static constexpr int ROOT = 3;

int mod_pow(int a, int e) {
    long long r = 1, x = a;
    while (e) {
        if (e & 1) r = r * x % MOD;
        x = x * x % MOD;
        e >>= 1;
    }
    return (int)r;
}

void ntt(vector<int>& a, bool invert) {
    const int n = (int)a.size();
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }
    for (int len = 2; len <= n; len <<= 1) {
        int wlen = mod_pow(ROOT, (MOD - 1) / len);
        if (invert) wlen = mod_pow(wlen, MOD - 2);
        for (int i = 0; i < n; i += len) {
            long long w = 1;
            for (int j = 0; j < len / 2; ++j) {
                int u = a[i + j];
                int v = (int)(a[i + j + len / 2] * w % MOD);
                a[i + j] = u + v < MOD ? u + v : u + v - MOD;
                a[i + j + len / 2] = u - v >= 0 ? u - v : u - v + MOD;
                w = w * wlen % MOD;
            }
        }
    }
    if (invert) {
        int inv_n = mod_pow(n, MOD - 2);
        for (int& x : a) x = (int)((long long)x * inv_n % MOD);
    }
}

vector<int> convolve(const vector<int>& a, const vector<int>& b) {
    int need = (int)a.size() + (int)b.size() - 1;
    int n = 1;
    while (n < need) n <<= 1;
    vector<int> x(n), y(n);
    copy(a.begin(), a.end(), x.begin());
    copy(b.begin(), b.end(), y.begin());
    ntt(x, false);
    ntt(y, false);
    for (int i = 0; i < n; ++i) x[i] = (int)((long long)x[i] * y[i] % MOD);
    ntt(x, true);
    x.resize(need);
    return x;
}

struct BFSResult {
    int farthest;
    vector<int> parent;
};

BFSResult bfs_farthest(const vector<vector<int>>& g, int start, bool keep_parent) {
    int n = (int)g.size();
    vector<int> parent(n, -1), dist(n, -1);
    queue<int> q;
    q.push(start);
    dist[start] = 0;
    int farthest = start;
    while (!q.empty()) {
        int v = q.front(); q.pop();
        if (dist[v] > dist[farthest]) farthest = v;
        for (int u : g[v]) if (dist[u] == -1) {
            dist[u] = dist[v] + 1;
            parent[u] = v;
            q.push(u);
        }
    }
    if (!keep_parent) parent.clear();
    return {farthest, move(parent)};
}

// Returns depths at which two (possibly identical) deepest vertices can
// have their lowest common ancestor, with the side root at depth zero.
vector<int> possible_prefixes(const vector<vector<int>>& g, int root, int blocked,
                              int h, vector<int>& parent, vector<int>& depth,
                              vector<int>& subtree_count) {
    fill(parent.begin(), parent.end(), -2);
    vector<int> order;
    order.reserve(g.size());
    vector<int> st = {root};
    parent[root] = -1;
    depth[root] = 0;
    while (!st.empty()) {
        int v = st.back(); st.pop_back();
        order.push_back(v);
        for (int u : g[v]) {
            if (u == parent[v] || (v == root && u == blocked)) continue;
            parent[u] = v;
            depth[u] = depth[v] + 1;
            st.push_back(u);
        }
    }

    vector<int> possible(h + 1, 0);
    for (int i = (int)order.size() - 1; i >= 0; --i) {
        int v = order[i];
        if (depth[v] == h) {
            subtree_count[v] = 1;
            possible[h] = 1; // Choosing the same endpoint twice is allowed.
            continue;
        }
        int count = 0, positive_children = 0;
        for (int u : g[v]) if (parent[u] == v) {
            count += subtree_count[u];
            if (subtree_count[u] > 0) ++positive_children;
        }
        subtree_count[v] = count;
        if (positive_children >= 2) possible[depth[v]] = 1;
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
        vector<vector<int>> g(n);
        for (int i = 1; i < n; ++i) {
            int u, v; cin >> u >> v; --u; --v;
            g[u].push_back(v);
            g[v].push_back(u);
        }

        int a = bfs_farthest(g, 0, false).farthest;
        BFSResult from_a = bfs_farthest(g, a, true);
        int b = from_a.farthest;
        vector<int> path;
        for (int v = b; v != -1; v = from_a.parent[v]) path.push_back(v);
        reverse(path.begin(), path.end());
        int diameter = (int)path.size() - 1;
        int h = diameter / 2;
        int left = path[h], right = path[h + 1];

        vector<int> parent(n), depth(n), subtree_count(n);
        vector<int> L = possible_prefixes(g, left, right, h, parent, depth, subtree_count);
        vector<int> R = possible_prefixes(g, right, left, h, parent, depth, subtree_count);

        vector<int> sums = convolve(L, R);
        vector<int> answer;
        for (int s = 0; s < (int)sums.size(); ++s)
            if (sums[s] != 0) answer.push_back(s + 1);

        cout << answer.size() << '\n';
        for (int i = 0; i < (int)answer.size(); ++i) {
            if (i) cout << ' ';
            cout << answer[i];
        }
        cout << '\n';
    }
}
