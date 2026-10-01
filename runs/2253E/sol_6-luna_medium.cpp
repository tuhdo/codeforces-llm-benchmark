#include <bits/stdc++.h>
using namespace std;

static constexpr int MOD = 998244353;
static constexpr int ROOT = 3;

int mod_pow(int a, int e) {
    long long r = 1;
    while (e) {
        if (e & 1) r = r * a % MOD;
        a = (long long)a * a % MOD;
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
        for (int& x : a) x = (long long)x * inv_n % MOD;
    }
}

pair<int, vector<int>> farthest(int start, const vector<vector<int>>& g) {
    int n = (int)g.size();
    vector<int> dist(n, -1), parent(n, -1), q;
    q.reserve(n);
    q.push_back(start); dist[start] = 0;
    int best = start;
    for (size_t i = 0; i < q.size(); ++i) {
        int v = q[i];
        if (dist[v] > dist[best]) best = v;
        for (int to : g[v]) if (dist[to] == -1) {
            dist[to] = dist[v] + 1;
            parent[to] = v;
            q.push_back(to);
        }
    }
    return {best, move(parent)};
}

vector<char> get_values(int root, int blocked, int h, const vector<vector<int>>& g) {
    int n = (int)g.size();
    vector<int> par(n, -2), dep(n, 0), order;
    order.reserve(n);
    par[root] = -1;
    order.push_back(root);
    for (size_t i = 0; i < order.size(); ++i) {
        int v = order[i];
        for (int to : g[v]) if (to != blocked && par[to] == -2) {
            par[to] = v;
            dep[to] = dep[v] + 1;
            order.push_back(to);
        }
    }
    vector<char> has(n, 0), values(h + 1, 0);
    for (int v : order) if (dep[v] == h) has[v] = 1;
    for (int i = (int)order.size() - 1; i >= 0; --i) {
        int v = order[i], branches = 0;
        for (int to : g[v]) if (par[to] == v && has[to]) ++branches;
        if (branches >= 2) values[dep[v]] = 1;
        if (has[v]) continue;
        if (branches > 0) has[v] = 1;
    }
    values[h] = 1; // Choosing the same diameter twice gives the full side path.
    return values;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    if (!(cin >> T)) return 0;
    while (T--) {
        int n; cin >> n;
        vector<vector<int>> g(n);
        for (int i = 0; i < n - 1; ++i) {
            int u, v; cin >> u >> v; --u; --v;
            g[u].push_back(v); g[v].push_back(u);
        }
        auto [u, p0] = farthest(0, g);
        auto [v, parent] = farthest(u, g);
        vector<int> path;
        for (int x = v; x != -1; x = parent[x]) path.push_back(x);
        reverse(path.begin(), path.end());
        int m = (int)path.size();
        int a = path[m / 2 - 1], b = path[m / 2];
        int h = m / 2 - 1;

        vector<char> A = get_values(a, b, h, g);
        vector<char> B = get_values(b, a, h, g);
        int need = 1;
        while (need < 2 * h + 1) need <<= 1;
        vector<int> fa(need), fb(need);
        for (int i = 0; i <= h; ++i) {
            fa[i] = A[i]; fb[i] = B[i];
        }
        ntt(fa, false); ntt(fb, false);
        for (int i = 0; i < need; ++i) fa[i] = (long long)fa[i] * fb[i] % MOD;
        ntt(fa, true);
        vector<int> ans;
        for (int s = 0; s <= 2 * h; ++s) if (fa[s] != 0) ans.push_back(s + 1);
        cout << ans.size() << '\n';
        for (int i = 0; i < (int)ans.size(); ++i) cout << ans[i] << (i + 1 == (int)ans.size() ? '\n' : ' ');
        if (ans.empty()) cout << '\n';
    }
    return 0;
}
