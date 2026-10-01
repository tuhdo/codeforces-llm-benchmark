#include <bits/stdc++.h>
using namespace std;

static constexpr int MOD = 998244353;
static constexpr int ROOT = 3;

int mod_pow(int a, int e) {
    long long result = 1;
    long long base = a;
    while (e > 0) {
        if (e & 1) result = result * base % MOD;
        base = base * base % MOD;
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
        const int half = len >> 1;
        for (int i = 0; i < n; i += len) {
            long long w = 1;
            for (int j = 0; j < half; ++j) {
                const int u = a[i + j];
                const int v = static_cast<int>(a[i + j + half] * w % MOD);
                int sum = u + v;
                if (sum >= MOD) sum -= MOD;
                int diff = u - v;
                if (diff < 0) diff += MOD;
                a[i + j] = sum;
                a[i + j + half] = diff;
                w = w * wlen % MOD;
            }
        }
    }

    if (invert) {
        const int inv_n = mod_pow(n, MOD - 2);
        for (int& x : a) x = static_cast<int>(static_cast<long long>(x) * inv_n % MOD);
    }
}

struct Tree {
    int n;
    vector<int> head, to, next;
    int edge_count = 0;

    explicit Tree(int n_) : n(n_), head(n_, -1), to(2 * (n_ - 1)), next(2 * (n_ - 1)) {}

    void add_edge(int u, int v) {
        to[edge_count] = v;
        next[edge_count] = head[u];
        head[u] = edge_count++;
        to[edge_count] = u;
        next[edge_count] = head[v];
        head[v] = edge_count++;
    }
};

pair<int, int> farthest_vertex(const Tree& tree, int start) {
    vector<int> dist(tree.n, -1), queue(tree.n);
    int first = 0, last = 0;
    queue[last++] = start;
    dist[start] = 0;
    int farthest = start;

    while (first < last) {
        const int u = queue[first++];
        if (dist[u] > dist[farthest]) farthest = u;
        for (int e = tree.head[u]; e != -1; e = tree.next[e]) {
            const int v = tree.to[e];
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                queue[last++] = v;
            }
        }
    }
    return {farthest, dist[farthest]};
}

tuple<int, int, int> find_middle_edge(const Tree& tree) {
    const int endpoint_a = farthest_vertex(tree, 0).first;

    vector<int> dist(tree.n, -1), parent(tree.n, -1), queue(tree.n);
    int first = 0, last = 0;
    queue[last++] = endpoint_a;
    dist[endpoint_a] = 0;
    int endpoint_b = endpoint_a;

    while (first < last) {
        const int u = queue[first++];
        if (dist[u] > dist[endpoint_b]) endpoint_b = u;
        for (int e = tree.head[u]; e != -1; e = tree.next[e]) {
            const int v = tree.to[e];
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                parent[v] = u;
                queue[last++] = v;
            }
        }
    }

    const int diameter = dist[endpoint_b];
    const int half = diameter / 2;
    int center_a = endpoint_b;
    for (int steps = 0; steps < half; ++steps) center_a = parent[center_a];
    const int center_b = parent[center_a];
    return {center_a, center_b, half};
}

vector<char> possible_lca_depths(const Tree& tree, int root, int blocked, int half) {
    vector<int> parent(tree.n, -2), depth(tree.n), order;
    order.reserve(tree.n);
    parent[root] = blocked;
    depth[root] = 0;
    order.push_back(root);

    for (size_t i = 0; i < order.size(); ++i) {
        const int u = order[i];
        for (int e = tree.head[u]; e != -1; e = tree.next[e]) {
            const int v = tree.to[e];
            if (v == parent[u]) continue;
            parent[v] = u;
            depth[v] = depth[u] + 1;
            order.push_back(v);
        }
    }

    vector<int> deep_endpoints(tree.n, 0);
    vector<char> possible(half + 1, false);
    possible[half] = true; // Choose the same diameter endpoint twice.

    for (int i = static_cast<int>(order.size()) - 1; i >= 0; --i) {
        const int u = order[i];
        if (depth[u] == half) {
            deep_endpoints[u] = 1;
            continue;
        }

        int endpoint_count = 0;
        int nonempty_children = 0;
        for (int e = tree.head[u]; e != -1; e = tree.next[e]) {
            const int v = tree.to[e];
            if (parent[v] == u && deep_endpoints[v] > 0) {
                ++nonempty_children;
                endpoint_count += deep_endpoints[v];
            }
        }
        if (depth[u] < half && nonempty_children >= 2) possible[depth[u]] = true;
        deep_endpoints[u] = min(2, endpoint_count);
    }
    return possible;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int test_cases;
    cin >> test_cases;
    while (test_cases--) {
        int n;
        cin >> n;
        Tree tree(n);
        for (int i = 0; i < n - 1; ++i) {
            int u, v;
            cin >> u >> v;
            tree.add_edge(--u, --v);
        }

        auto [center_a, center_b, half] = find_middle_edge(tree);
        const vector<char> left = possible_lca_depths(tree, center_a, center_b, half);
        const vector<char> right = possible_lca_depths(tree, center_b, center_a, half);

        int transform_size = 1;
        while (transform_size < 2 * half + 1) transform_size <<= 1;
        vector<int> a(transform_size), b(transform_size);
        for (int d = 0; d <= half; ++d) {
            a[d] = left[d];
            b[d] = right[d];
        }

        ntt(a, false);
        ntt(b, false);
        for (int i = 0; i < transform_size; ++i)
            a[i] = static_cast<int>(static_cast<long long>(a[i]) * b[i] % MOD);
        ntt(a, true);

        vector<int> beautiful;
        for (int sum = 0; sum <= 2 * half; ++sum)
            if (a[sum] != 0) beautiful.push_back(sum + 1);

        cout << beautiful.size();
        for (int k : beautiful) cout << ' ' << k;
        cout << '\n';
    }
    return 0;
}
