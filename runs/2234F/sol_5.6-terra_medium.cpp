#include <bits/stdc++.h>
using namespace std;

struct DSU {
    vector<int> parent, size, left, right;

    explicit DSU(int n) : parent(n), size(n, 1), left(n), right(n) {
        iota(parent.begin(), parent.end(), 0);
        iota(left.begin(), left.end(), 0);
        iota(right.begin(), right.end(), 0);
    }

    int find(int x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    }

    int unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (size[a] < size[b]) swap(a, b);
        parent[b] = a;
        size[a] += size[b];
        left[a] = min(left[a], left[b]);
        right[a] = max(right[a], right[b]);
        return a;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        vector<long long> h(n);
        for (long long &x : h) cin >> x;

        int removed = max_element(h.begin(), h.end()) - h.begin();

        // Position 0 is the vertex immediately after the removed edge.
        // The remaining cycle edges form a path between positions p and p + 1.
        vector<pair<long long, int>> edges;
        edges.reserve(n - 1);
        for (int p = 0; p + 1 < n; ++p) {
            edges.push_back({h[(removed + 1 + p) % n], p});
        }
        sort(edges.begin(), edges.end());

        DSU dsu(n);
        vector<long long> difference(n + 1, 0);
        auto add_range = [&](int l, int r, long long value) {
            difference[l] += value;
            difference[r + 1] -= value;
        };

        // When an edge of height w joins components A and B, exactly the
        // pairs crossing A and B get minimax value w.
        for (auto [weight, p] : edges) {
            int a = dsu.find(p);
            int b = dsu.find(p + 1);
            add_range(dsu.left[a], dsu.right[a], weight * dsu.size[b]);
            add_range(dsu.left[b], dsu.right[b], weight * dsu.size[a]);
            dsu.unite(a, b);
        }

        vector<long long> answer(n);
        long long current = 0;
        for (int p = 0; p < n; ++p) {
            current += difference[p];
            answer[(removed + 1 + p) % n] = current;
        }

        for (int i = 0; i < n; ++i) {
            if (i) cout << ' ';
            cout << answer[i];
        }
        cout << '\n';
    }
}
