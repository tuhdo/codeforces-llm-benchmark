#include <bits/stdc++.h>
using namespace std;

namespace {

constexpr int MOD = 998244353;
constexpr int ROOT = 3;

int modPow(int a, int e) {
    int result = 1;
    while (e > 0) {
        if (e & 1) result = static_cast<long long>(result) * a % MOD;
        a = static_cast<long long>(a) * a % MOD;
        e >>= 1;
    }
    return result;
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
        int wlen = modPow(ROOT, (MOD - 1) / len);
        if (invert) wlen = modPow(wlen, MOD - 2);

        for (int start = 0; start < n; start += len) {
            long long w = 1;
            const int half = len >> 1;
            for (int i = 0; i < half; ++i) {
                const int u = a[start + i];
                const int v = static_cast<int>(a[start + i + half] * w % MOD);
                a[start + i] = u + v < MOD ? u + v : u + v - MOD;
                a[start + i + half] = u - v >= 0 ? u - v : u - v + MOD;
                w = w * wlen % MOD;
            }
        }
    }

    if (invert) {
        const int invN = modPow(n, MOD - 2);
        for (int& x : a) x = static_cast<long long>(x) * invN % MOD;
    }
}

pair<int, vector<int>> farthestFrom(int source, const vector<vector<int>>& graph) {
    const int n = static_cast<int>(graph.size()) - 1;
    vector<int> parent(n + 1, -1);
    vector<int> order;
    order.reserve(n);
    order.push_back(source);
    parent[source] = 0;

    for (int i = 0; i < static_cast<int>(order.size()); ++i) {
        const int v = order[i];
        for (int to : graph[v]) {
            if (to == parent[v]) continue;
            parent[to] = v;
            order.push_back(to);
        }
    }

    return {order.back(), move(parent)};
}

vector<char> possibleLcaDepths(
    int root,
    int blocked,
    int height,
    const vector<vector<int>>& graph
) {
    const int n = static_cast<int>(graph.size()) - 1;
    vector<int> parent(n + 1, -1);
    vector<int> depth(n + 1, 0);
    vector<int> order;
    order.push_back(root);
    parent[root] = blocked;

    for (int i = 0; i < static_cast<int>(order.size()); ++i) {
        const int v = order[i];
        for (int to : graph[v]) {
            if (to == parent[v]) continue;
            parent[to] = v;
            depth[to] = depth[v] + 1;
            order.push_back(to);
        }
    }

    vector<char> hasDeepest(n + 1, false);
    vector<char> possible(height + 1, false);
    possible[height] = true;

    for (int i = static_cast<int>(order.size()) - 1; i >= 0; --i) {
        const int v = order[i];
        if (depth[v] == height) {
            hasDeepest[v] = true;
            continue;
        }

        int deepChildren = 0;
        for (int to : graph[v]) {
            if (parent[to] != v) continue;
            if (hasDeepest[to]) {
                hasDeepest[v] = true;
                ++deepChildren;
            }
        }
        if (deepChildren >= 2) possible[depth[v]] = true;
    }

    return possible;
}

}  // namespace

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        vector<vector<int>> graph(n + 1);
        for (int i = 0; i + 1 < n; ++i) {
            int u, v;
            cin >> u >> v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }

        const auto [oneEnd, ignoredParents] = farthestFrom(1, graph);
        const auto [otherEnd, parent] = farthestFrom(oneEnd, graph);

        vector<int> diameter;
        for (int v = otherEnd; v != 0; v = parent[v]) diameter.push_back(v);
        reverse(diameter.begin(), diameter.end());

        const int diameterLength = static_cast<int>(diameter.size()) - 1;
        const int height = diameterLength / 2;
        const int leftCenter = diameter[height];
        const int rightCenter = diameter[height + 1];

        const vector<char> left = possibleLcaDepths(leftCenter, rightCenter, height, graph);
        const vector<char> right = possibleLcaDepths(rightCenter, leftCenter, height, graph);

        int transformSize = 1;
        while (transformSize <= 2 * height) transformSize <<= 1;
        vector<int> a(transformSize), b(transformSize);
        for (int i = 0; i <= height; ++i) {
            a[i] = left[i];
            b[i] = right[i];
        }

        ntt(a, false);
        ntt(b, false);
        for (int i = 0; i < transformSize; ++i) {
            a[i] = static_cast<long long>(a[i]) * b[i] % MOD;
        }
        ntt(a, true);

        vector<int> answer;
        for (int sum = 0; sum <= 2 * height; ++sum) {
            if (a[sum] != 0) answer.push_back(sum + 1);
        }

        cout << answer.size();
        for (int k : answer) cout << ' ' << k;
        cout << '\n';
    }
}
