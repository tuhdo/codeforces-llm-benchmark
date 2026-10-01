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
        for (int start = 0; start < n; start += len) {
            long long w = 1;
            const int half = len >> 1;
            for (int j = 0; j < half; ++j) {
                int u = a[start + j];
                int v = static_cast<int>(w * a[start + j + half] % MOD);
                int sum = u + v;
                if (sum >= MOD) sum -= MOD;
                int diff = u - v;
                if (diff < 0) diff += MOD;
                a[start + j] = sum;
                a[start + j + half] = diff;
                w = w * wlen % MOD;
            }
        }
    }

    if (invert) {
        int inv_n = mod_pow(n, MOD - 2);
        for (int& x : a) x = static_cast<int>(1LL * x * inv_n % MOD);
    }
}

struct Tree {
    int n;
    vector<int> head, to, next;

    explicit Tree(int n) : n(n), head(n, -1) {
        to.reserve(2 * (n - 1));
        next.reserve(2 * (n - 1));
    }

    void add_edge(int u, int v) {
        to.push_back(v);
        next.push_back(head[u]);
        head[u] = static_cast<int>(to.size()) - 1;

        to.push_back(u);
        next.push_back(head[v]);
        head[v] = static_cast<int>(to.size()) - 1;
    }
};

int farthest_vertex(const Tree& tree, int source, vector<int>& parent,
                    vector<int>& distance) {
    fill(parent.begin(), parent.end(), -1);
    fill(distance.begin(), distance.end(), -1);
    vector<int> stack;
    stack.reserve(tree.n);
    stack.push_back(source);
    parent[source] = source;
    distance[source] = 0;
    int farthest = source;

    while (!stack.empty()) {
        int u = stack.back();
        stack.pop_back();
        if (distance[u] > distance[farthest]) farthest = u;

        for (int e = tree.head[u]; e != -1; e = tree.next[e]) {
            int v = tree.to[e];
            if (v == parent[u]) continue;
            parent[v] = u;
            distance[v] = distance[u] + 1;
            stack.push_back(v);
        }
    }
    return farthest;
}

vector<unsigned char> possible_common_depths(const Tree& tree, int root,
                                             int blocked_neighbor, int radius) {
    vector<int> parent(tree.n, -2), depth(tree.n, 0), order;
    order.reserve(tree.n / 2 + 1);
    vector<int> stack;
    stack.reserve(tree.n / 2 + 1);
    parent[root] = -1;
    stack.push_back(root);

    while (!stack.empty()) {
        int u = stack.back();
        stack.pop_back();
        order.push_back(u);
        for (int e = tree.head[u]; e != -1; e = tree.next[e]) {
            int v = tree.to[e];
            if (v == blocked_neighbor || v == parent[u]) continue;
            parent[v] = u;
            depth[v] = depth[u] + 1;
            stack.push_back(v);
        }
    }

    vector<unsigned char> active_child_count(tree.n, 0);
    vector<unsigned char> possible(radius + 1, 0);
    possible[radius] = 1;  // A diameter may use the same endpoint twice.

    for (int i = static_cast<int>(order.size()) - 1; i >= 0; --i) {
        int u = order[i];
        bool has_diameter_endpoint = (depth[u] == radius || active_child_count[u] > 0);

        if (depth[u] < radius && active_child_count[u] >= 2) {
            possible[depth[u]] = 1;
        }

        if (parent[u] != -1 && has_diameter_endpoint) {
            int p = parent[u];
            if (active_child_count[p] < 2) ++active_child_count[p];
        }
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
            --u;
            --v;
            tree.add_edge(u, v);
        }

        vector<int> parent(n), distance(n);
        int endpoint_a = farthest_vertex(tree, 0, parent, distance);
        int endpoint_b = farthest_vertex(tree, endpoint_a, parent, distance);
        int diameter = distance[endpoint_b];

        vector<int> path;
        for (int v = endpoint_b; v != endpoint_a; v = parent[v]) path.push_back(v);
        path.push_back(endpoint_a);
        reverse(path.begin(), path.end());

        int radius = diameter / 2;
        int center_left = path[radius];
        int center_right = path[radius + 1];

        vector<unsigned char> left_depths =
            possible_common_depths(tree, center_left, center_right, radius);
        vector<unsigned char> right_depths =
            possible_common_depths(tree, center_right, center_left, radius);

        int needed = 2 * radius + 1;
        int transform_size = 1;
        while (transform_size < needed) transform_size <<= 1;

        vector<int> left_poly(transform_size, 0), right_poly(transform_size, 0);
        for (int d = 0; d <= radius; ++d) {
            left_poly[d] = left_depths[d];
            right_poly[d] = right_depths[d];
        }

        ntt(left_poly, false);
        ntt(right_poly, false);
        for (int i = 0; i < transform_size; ++i) {
            left_poly[i] = static_cast<int>(1LL * left_poly[i] * right_poly[i] % MOD);
        }
        ntt(left_poly, true);

        vector<int> answer;
        for (int sum = 0; sum <= 2 * radius; ++sum) {
            if (left_poly[sum] != 0) answer.push_back(sum + 1);
        }
        cout << answer.size() << '\n';
        for (int i = 0; i < static_cast<int>(answer.size()); ++i) {
            if (i) cout << ' ';
            cout << answer[i];
        }
        cout << '\n';
    }
    return 0;
}
