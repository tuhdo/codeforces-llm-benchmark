#include <bits/stdc++.h>
using namespace std;

static constexpr int MOD = 998244353;
static constexpr int ROOT = 3;

int mod_pow(int a, int e) {
    long long result = 1;
    while (e > 0) {
        if (e & 1) result = result * a % MOD;
        a = static_cast<int>(1LL * a * a % MOD);
        e >>= 1;
    }
    return static_cast<int>(result);
}

void ntt(vector<int>& a, bool invert) {
    const int n = static_cast<int>(a.size());
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
            const int half = len >> 1;
            for (int j = 0; j < half; ++j) {
                int u = a[i + j];
                int v = static_cast<int>(a[i + j + half] * w % MOD);
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
        for (int& x : a) x = static_cast<int>(1LL * x * inv_n % MOD);
    }
}

vector<int> collect_depths(int root, int blocked, int h,
                           const vector<int>& head, const vector<int>& to,
                           const vector<int>& next_edge, vector<int>& parent,
                           vector<int>& depth, vector<int>& order,
                           vector<unsigned char>& has_deep) {
    order.clear();
    order.reserve(order.capacity());
    vector<int> stack;
    stack.push_back(root);
    parent[root] = blocked;
    depth[root] = 0;

    while (!stack.empty()) {
        int v = stack.back();
        stack.pop_back();
        order.push_back(v);
        for (int e = head[v]; e != -1; e = next_edge[e]) {
            int u = to[e];
            if (u == parent[v]) continue;
            parent[u] = v;
            depth[u] = depth[v] + 1;
            stack.push_back(u);
        }
    }

    vector<int> result;
    vector<unsigned char> possible(h + 1, 0);
    possible[h] = 1;

    for (int i = static_cast<int>(order.size()) - 1; i >= 0; --i) {
        int v = order[i];
        int deep_child_count = 0;
        for (int e = head[v]; e != -1; e = next_edge[e]) {
            int u = to[e];
            if (u == blocked) continue;
            if (parent[u] == v && has_deep[u]) ++deep_child_count;
        }

        has_deep[v] = static_cast<unsigned char>(depth[v] == h || deep_child_count > 0);
        if (deep_child_count >= 2) possible[depth[v]] = 1;
    }

    for (int d = 0; d <= h; ++d) {
        if (possible[d]) result.push_back(d);
    }
    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;

        vector<int> head(n + 1, -1);
        vector<int> to(2 * (n - 1));
        vector<int> next_edge(2 * (n - 1));
        int edge_count = 0;

        auto add_edge = [&](int u, int v) {
            to[edge_count] = v;
            next_edge[edge_count] = head[u];
            head[u] = edge_count++;
        };

        for (int i = 0; i < n - 1; ++i) {
            int u, v;
            cin >> u >> v;
            add_edge(u, v);
            add_edge(v, u);
        }

        vector<int> parent(n + 1), depth(n + 1), order;

        auto farthest = [&](int start, bool keep_parent) {
            order.clear();
            vector<int> stack;
            stack.push_back(start);
            parent[start] = 0;
            depth[start] = 0;
            int best = start;

            while (!stack.empty()) {
                int v = stack.back();
                stack.pop_back();
                order.push_back(v);
                if (depth[v] > depth[best]) best = v;
                for (int e = head[v]; e != -1; e = next_edge[e]) {
                    int u = to[e];
                    if (u == parent[v]) continue;
                    parent[u] = v;
                    depth[u] = depth[v] + 1;
                    stack.push_back(u);
                }
            }
            if (!keep_parent) return best;
            return best;
        };

        int endpoint_a = farthest(1, false);
        int endpoint_b = farthest(endpoint_a, true);
        int diameter = depth[endpoint_b];
        int h = diameter / 2;

        vector<int> path;
        for (int v = endpoint_b;; v = parent[v]) {
            path.push_back(v);
            if (v == endpoint_a) break;
        }

        int center_b_side = path[h];
        int center_a_side = path[h + 1];
        vector<unsigned char> has_deep(n + 1, 0);

        vector<int> left = collect_depths(
            center_b_side, center_a_side, h, head, to, next_edge,
            parent, depth, order, has_deep);
        vector<int> right = collect_depths(
            center_a_side, center_b_side, h, head, to, next_edge,
            parent, depth, order, has_deep);

        int convolution_size = 1;
        while (convolution_size <= 2 * h) convolution_size <<= 1;
        vector<int> a(convolution_size, 0), b(convolution_size, 0);
        for (int x : left) a[x] = 1;
        for (int x : right) b[x] = 1;

        ntt(a, false);
        ntt(b, false);
        for (int i = 0; i < convolution_size; ++i) {
            a[i] = static_cast<int>(1LL * a[i] * b[i] % MOD);
        }
        ntt(a, true);

        vector<int> answer;
        for (int sum = 0; sum <= 2 * h; ++sum) {
            if (a[sum] != 0) answer.push_back(sum + 1);
        }

        cout << answer.size();
        for (int x : answer) cout << ' ' << x;
        cout << '\n';
    }
    return 0;
}
