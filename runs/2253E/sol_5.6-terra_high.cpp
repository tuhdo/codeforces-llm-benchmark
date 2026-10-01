#include <bits/stdc++.h>
using namespace std;

constexpr int MOD = 998244353;
constexpr int PRIMITIVE_ROOT = 3;

int mod_pow(int base, int exponent) {
    int result = 1;
    while (exponent > 0) {
        if (exponent & 1) result = 1LL * result * base % MOD;
        base = 1LL * base * base % MOD;
        exponent >>= 1;
    }
    return result;
}

void ntt(vector<int>& values, bool invert) {
    const int n = static_cast<int>(values.size());
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(values[i], values[j]);
    }

    for (int length = 2; length <= n; length <<= 1) {
        int root = mod_pow(PRIMITIVE_ROOT, (MOD - 1) / length);
        if (invert) root = mod_pow(root, MOD - 2);
        for (int start = 0; start < n; start += length) {
            long long multiplier = 1;
            const int half = length >> 1;
            for (int i = 0; i < half; ++i) {
                int left = values[start + i];
                int right = multiplier * values[start + i + half] % MOD;
                values[start + i] = left + right < MOD ? left + right : left + right - MOD;
                values[start + i + half] = left - right >= 0 ? left - right : left - right + MOD;
                multiplier = multiplier * root % MOD;
            }
        }
    }

    if (invert) {
        const int inverse_n = mod_pow(n, MOD - 2);
        for (int& value : values) value = 1LL * value * inverse_n % MOD;
    }
}

vector<int> farthest_from(int start, const vector<vector<int>>& graph, vector<int>* parent_out = nullptr) {
    const int n = static_cast<int>(graph.size()) - 1;
    vector<int> parent(n + 1, -1), distance(n + 1, -1);
    vector<int> stack = {start};
    parent[start] = 0;
    distance[start] = 0;
    int farthest = start;

    while (!stack.empty()) {
        int vertex = stack.back();
        stack.pop_back();
        if (distance[vertex] > distance[farthest]) farthest = vertex;
        for (int next : graph[vertex]) {
            if (next == parent[vertex]) continue;
            parent[next] = vertex;
            distance[next] = distance[vertex] + 1;
            stack.push_back(next);
        }
    }

    if (parent_out != nullptr) *parent_out = move(parent);
    return {farthest, distance[farthest]};
}

vector<int> possible_prefix_lengths(
    int root,
    int blocked_neighbor,
    int radius,
    const vector<vector<int>>& graph,
    vector<int>& parent,
    vector<int>& depth,
    vector<int>& child_count,
    vector<unsigned char>& reaches_bottom
) {
    vector<int> order;
    order.reserve(graph.size() / 2);
    vector<int> stack = {root};
    parent[root] = 0;
    depth[root] = 0;

    while (!stack.empty()) {
        int vertex = stack.back();
        stack.pop_back();
        order.push_back(vertex);
        child_count[vertex] = 0;
        for (int next : graph[vertex]) {
            if (next == parent[vertex] || next == blocked_neighbor) continue;
            parent[next] = vertex;
            depth[next] = depth[vertex] + 1;
            stack.push_back(next);
        }
    }

    vector<int> possible(radius + 1, 0);
    possible[radius] = 1;  // Choosing the same endpoint twice gives the full prefix.
    for (int index = static_cast<int>(order.size()) - 1; index >= 0; --index) {
        int vertex = order[index];
        reaches_bottom[vertex] = (depth[vertex] == radius || child_count[vertex] > 0);
        if (child_count[vertex] >= 2) possible[depth[vertex]] = 1;
        if (reaches_bottom[vertex] && parent[vertex] != 0) ++child_count[parent[vertex]];
    }
    return possible;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int test_count;
    cin >> test_count;
    while (test_count--) {
        int n;
        cin >> n;
        vector<vector<int>> graph(n + 1);
        for (int i = 1; i < n; ++i) {
            int u, v;
            cin >> u >> v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }

        int endpoint_a = farthest_from(1, graph)[0];
        vector<int> diameter_parent;
        vector<int> farthest = farthest_from(endpoint_a, graph, &diameter_parent);
        int endpoint_b = farthest[0];
        int diameter_length = farthest[1];
        int radius = diameter_length / 2;

        vector<int> diameter_path;
        for (int vertex = endpoint_b; vertex != 0; vertex = diameter_parent[vertex]) {
            diameter_path.push_back(vertex);
        }
        reverse(diameter_path.begin(), diameter_path.end());
        int center_left = diameter_path[radius];
        int center_right = diameter_path[radius + 1];

        vector<int> parent(n + 1), depth(n + 1), child_count(n + 1);
        vector<unsigned char> reaches_bottom(n + 1);
        vector<int> left = possible_prefix_lengths(
            center_left, center_right, radius, graph, parent, depth, child_count, reaches_bottom
        );
        vector<int> right = possible_prefix_lengths(
            center_right, center_left, radius, graph, parent, depth, child_count, reaches_bottom
        );

        int transform_size = 1;
        while (transform_size <= 2 * radius) transform_size <<= 1;
        left.resize(transform_size);
        right.resize(transform_size);
        ntt(left, false);
        ntt(right, false);
        for (int i = 0; i < transform_size; ++i) left[i] = 1LL * left[i] * right[i] % MOD;
        ntt(left, true);

        vector<int> answer;
        for (int sum = 0; sum <= 2 * radius; ++sum) {
            if (left[sum] != 0) answer.push_back(sum + 1);
        }
        cout << answer.size();
        for (int value : answer) cout << ' ' << value;
        cout << '\n';
    }
}
