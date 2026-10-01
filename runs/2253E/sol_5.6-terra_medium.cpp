#include <bits/stdc++.h>
using namespace std;

constexpr int MOD = 998244353;
constexpr int ROOT = 3;

int mod_pow(int a, int e) {
    int result = 1;
    while (e) {
        if (e & 1) result = int64_t(result) * a % MOD;
        a = int64_t(a) * a % MOD;
        e >>= 1;
    }
    return result;
}

void ntt(vector<int>& a, bool invert) {
    int n = int(a.size());
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
            int w = 1;
            int half = len >> 1;
            for (int j = 0; j < half; ++j) {
                int u = a[start + j];
                int v = int64_t(a[start + j + half]) * w % MOD;
                a[start + j] = u + v < MOD ? u + v : u + v - MOD;
                a[start + j + half] = u - v >= 0 ? u - v : u - v + MOD;
                w = int64_t(w) * wlen % MOD;
            }
        }
    }
    if (invert) {
        int inv_n = mod_pow(n, MOD - 2);
        for (int& x : a) x = int64_t(x) * inv_n % MOD;
    }
}

vector<int> multiply(vector<int> a, vector<int> b) {
    int wanted = int(a.size() + b.size() - 1);
    int n = 1;
    while (n < wanted) n <<= 1;
    a.resize(n);
    b.resize(n);
    ntt(a, false);
    ntt(b, false);
    for (int i = 0; i < n; ++i) a[i] = int64_t(a[i]) * b[i] % MOD;
    ntt(a, true);
    a.resize(wanted);
    return a;
}

pair<int, vector<int>> farthest(int source, const vector<vector<int>>& graph, bool keep_parent) {
    int n = int(graph.size());
    vector<int> dist(n, -1), parent(keep_parent ? n : 0, -1);
    queue<int> q;
    q.push(source);
    dist[source] = 0;
    int last = source;
    while (!q.empty()) {
        int v = q.front(); q.pop();
        last = v;
        for (int to : graph[v]) if (dist[to] == -1) {
            dist[to] = dist[v] + 1;
            if (keep_parent) parent[to] = v;
            q.push(to);
        }
    }
    return {last, move(parent)};
}

vector<int> meeting_depths(int center, int blocked, int radius,
                           const vector<vector<int>>& graph) {
    int n = int(graph.size());
    vector<int> parent(n, -2), depth(n), order;
    order.reserve(n);
    parent[center] = -1;
    vector<int> stack = {center};
    while (!stack.empty()) {
        int v = stack.back(); stack.pop_back();
        order.push_back(v);
        for (int to : graph[v]) {
            if (to == blocked || to == parent[v]) continue;
            parent[to] = v;
            depth[to] = depth[v] + 1;
            stack.push_back(to);
        }
    }

    vector<int> answer(radius + 1), deep_children(n);
    vector<char> has_deep(n);
    for (int it = int(order.size()) - 1; it >= 0; --it) {
        int v = order[it];
        if (depth[v] == radius) has_deep[v] = true;
        if (depth[v] < radius && deep_children[v] >= 2) answer[depth[v]] = 1;
        if (deep_children[v] > 0) has_deep[v] = true;
        if (parent[v] != -1 && has_deep[v]) ++deep_children[parent[v]];
    }
    answer[radius] = 1; // Choose the same endpoint twice.
    return answer;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        vector<vector<int>> graph(n);
        for (int i = 1; i < n; ++i) {
            int u, v;
            cin >> u >> v;
            --u; --v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }

        int endpoint = farthest(0, graph, false).first;
        auto [other_endpoint, parent] = farthest(endpoint, graph, true);

        vector<int> diameter;
        for (int v = other_endpoint; v != -1; v = parent[v]) diameter.push_back(v);
        int length = int(diameter.size()) - 1;
        int radius = (length - 1) / 2;
        int left_center = diameter[radius];
        int right_center = diameter[radius + 1];

        vector<int> left = meeting_depths(left_center, right_center, radius, graph);
        vector<int> right = meeting_depths(right_center, left_center, radius, graph);
        vector<int> sums = multiply(left, right);

        vector<int> beautiful;
        for (int sum = 0; sum < int(sums.size()); ++sum) {
            if (sums[sum] != 0) beautiful.push_back(sum + 1);
        }
        cout << beautiful.size();
        for (int k : beautiful) cout << ' ' << k;
        cout << '\n';
    }
}
