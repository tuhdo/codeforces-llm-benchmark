#include <bits/stdc++.h>
using namespace std;

static constexpr int MOD = 998244353;
static constexpr int PRIMITIVE_ROOT = 3;

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
        int wlen = mod_pow(PRIMITIVE_ROOT, (MOD - 1) / len);
        if (invert) wlen = mod_pow(wlen, MOD - 2);

        for (int start = 0; start < n; start += len) {
            long long w = 1;
            const int half = len >> 1;
            for (int j = 0; j < half; ++j) {
                int u = a[start + j];
                int v = static_cast<int>(a[start + j + half] * w % MOD);

                int x = u + v;
                if (x >= MOD) x -= MOD;
                int y = u - v;
                if (y < 0) y += MOD;
                a[start + j] = x;
                a[start + j + half] = y;
                w = w * wlen % MOD;
            }
        }
    }

    if (invert) {
        const int inv_n = mod_pow(n, MOD - 2);
        for (int& x : a) x = static_cast<int>(1LL * x * inv_n % MOD);
    }
}

vector<int> convolution(vector<int> a, vector<int> b) {
    const int need = static_cast<int>(a.size() + b.size() - 1);
    int n = 1;
    while (n < need) n <<= 1;

    a.resize(n);
    b.resize(n);
    ntt(a, false);
    ntt(b, false);
    for (int i = 0; i < n; ++i) {
        a[i] = static_cast<int>(1LL * a[i] * b[i] % MOD);
    }
    ntt(a, true);
    a.resize(need);
    return a;
}

struct Graph {
    vector<int> head;
    vector<int> to;
    vector<int> next_edge;
    int edge_count = 0;

    explicit Graph(int n) : head(n, -1), to(2 * (n - 1)), next_edge(2 * (n - 1)) {}

    void add_edge(int u, int v) {
        to[edge_count] = v;
        next_edge[edge_count] = head[u];
        head[u] = edge_count++;
    }

    void add_undirected(int u, int v) {
        add_edge(u, v);
        add_edge(v, u);
    }
};

int farthest_vertex(
    int start,
    const Graph& graph,
    vector<int>& distance,
    vector<int>* parent = nullptr
) {
    const int n = static_cast<int>(graph.head.size());
    distance.assign(n, -1);
    if (parent != nullptr) parent->assign(n, -1);

    vector<int> stack;
    stack.reserve(n);
    stack.push_back(start);
    distance[start] = 0;
    int farthest = start;

    while (!stack.empty()) {
        int u = stack.back();
        stack.pop_back();
        if (distance[u] > distance[farthest]) farthest = u;

        for (int e = graph.head[u]; e != -1; e = graph.next_edge[e]) {
            int v = graph.to[e];
            if (distance[v] != -1) continue;
            distance[v] = distance[u] + 1;
            if (parent != nullptr) (*parent)[v] = u;
            stack.push_back(v);
        }
    }

    return farthest;
}

vector<char> attainable_side_depths(
    const Graph& graph,
    int root,
    int blocked,
    int radius
) {
    const int n = static_cast<int>(graph.head.size());
    vector<int> parent(n, -1);
    vector<int> depth(n, 0);
    vector<int> order;
    vector<int> stack;
    order.reserve(n);
    stack.reserve(n);

    parent[root] = blocked;
    stack.push_back(root);
    while (!stack.empty()) {
        int u = stack.back();
        stack.pop_back();
        order.push_back(u);

        for (int e = graph.head[u]; e != -1; e = graph.next_edge[e]) {
            int v = graph.to[e];
            if (v == parent[u]) continue;
            parent[v] = u;
            depth[v] = depth[u] + 1;
            stack.push_back(v);
        }
    }

    vector<char> has_deep_endpoint(n, 0);
    vector<char> attainable(radius + 1, 0);
    attainable[radius] = 1;

    for (int i = static_cast<int>(order.size()) - 1; i >= 0; --i) {
        int u = order[i];
        int child_branches = 0;

        for (int e = graph.head[u]; e != -1; e = graph.next_edge[e]) {
            int v = graph.to[e];
            if (parent[v] == u && has_deep_endpoint[v]) {
                ++child_branches;
            }
        }

        has_deep_endpoint[u] = (depth[u] == radius || child_branches > 0);
        if (child_branches >= 2 && depth[u] <= radius) {
            attainable[depth[u]] = 1;
        }
    }

    return attainable;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int test_cases;
    cin >> test_cases;
    while (test_cases--) {
        int n;
        cin >> n;

        Graph graph(n);
        for (int i = 0; i < n - 1; ++i) {
            int u, v;
            cin >> u >> v;
            --u;
            --v;
            graph.add_undirected(u, v);
        }

        vector<int> distance;
        int endpoint_a = farthest_vertex(0, graph, distance);

        vector<int> parent;
        int endpoint_b = farthest_vertex(endpoint_a, graph, distance, &parent);
        int diameter = distance[endpoint_b];
        int radius = diameter / 2;

        vector<int> diameter_path;
        for (int v = endpoint_b; v != -1; v = parent[v]) {
            diameter_path.push_back(v);
        }
        reverse(diameter_path.begin(), diameter_path.end());

        int center_left = diameter_path[radius];
        int center_right = diameter_path[radius + 1];

        vector<char> left_depths = attainable_side_depths(
            graph, center_left, center_right, radius
        );
        vector<char> right_depths = attainable_side_depths(
            graph, center_right, center_left, radius
        );

        vector<int> left_polynomial(radius + 1, 0);
        vector<int> right_polynomial(radius + 1, 0);
        for (int i = 0; i <= radius; ++i) {
            left_polynomial[i] = left_depths[i];
            right_polynomial[i] = right_depths[i];
        }

        vector<int> sums = convolution(move(left_polynomial), move(right_polynomial));

        vector<int> answer;
        answer.reserve(sums.size());
        for (int sum = 0; sum < static_cast<int>(sums.size()); ++sum) {
            if (sums[sum] != 0) answer.push_back(sum + 1);
        }

        cout << answer.size();
        for (int value : answer) cout << ' ' << value;
        cout << '\n';
    }

    return 0;
}
