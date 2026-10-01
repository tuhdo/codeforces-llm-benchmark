#include <bits/stdc++.h>
using namespace std;

struct DSU {
    vector<int> parent, size, left, right;

    explicit DSU(int n) : parent(n), size(n, 1), left(n), right(n) {
        iota(parent.begin(), parent.end(), 0);
        iota(left.begin(), left.end(), 0);
        iota(right.begin(), right.end(), 0);
    }

    int find(int v) {
        if (parent[v] == v) return v;
        return parent[v] = find(parent[v]);
    }

    void unite(int a, int b) {
        if (size[a] < size[b]) swap(a, b);
        parent[b] = a;
        size[a] += size[b];
        left[a] = min(left[a], left[b]);
        right[a] = max(right[a], right[b]);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<int> h(n);
        for (int &x : h) cin >> x;

        int cut = max_element(h.begin(), h.end()) - h.begin();
        int start = (cut + 1) % n;
        vector<pair<int, int>> edges;
        edges.reserve(n - 1);
        for (int i = 0; i < n - 1; ++i) {
            edges.emplace_back(h[(start + i) % n], i);
        }
        sort(edges.begin(), edges.end());

        DSU dsu(n);
        vector<long long> diff(n + 1, 0), answer(n);
        auto add = [&](int component, long long value) {
            diff[dsu.left[component]] += value;
            diff[dsu.right[component] + 1] -= value;
        };

        for (auto [height, position] : edges) {
            int a = dsu.find(position);
            int b = dsu.find(position + 1);
            add(a, 1LL * height * dsu.size[b]);
            add(b, 1LL * height * dsu.size[a]);
            dsu.unite(a, b);
        }

        long long current = 0;
        for (int i = 0; i < n; ++i) {
            current += diff[i];
            answer[(start + i) % n] = current;
        }
        for (int i = 0; i < n; ++i) {
            cout << answer[i] << (i + 1 == n ? '\n' : ' ');
        }
    }
}
