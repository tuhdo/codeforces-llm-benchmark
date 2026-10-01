#include <bits/stdc++.h>
using namespace std;

struct DSU {
    vector<int> parent, size, node;

    explicit DSU(int n) : parent(n), size(n, 1), node(n) {
        iota(parent.begin(), parent.end(), 0);
        iota(node.begin(), node.end(), 0);
    }

    int find(int v) {
        if (parent[v] != v) parent[v] = find(parent[v]);
        return parent[v];
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
        vector<pair<int, int>> edges(n);
        for (int i = 0; i < n; ++i) {
            cin >> edges[i].first;
            edges[i].second = i;
        }
        sort(edges.begin(), edges.end());

        DSU dsu(n);
        vector<int> tree_parent(2 * n - 1, -1);
        vector<long long> contribution(2 * n - 1, 0);
        int nodes = n;

        for (auto [height, i] : edges) {
            int a = dsu.find(i);
            int b = dsu.find((i + 1) % n);
            if (a == b) continue;

            int left = dsu.node[a];
            int right = dsu.node[b];
            int merged = nodes++;
            tree_parent[left] = tree_parent[right] = merged;
            contribution[left] = 1LL * height * dsu.size[b];
            contribution[right] = 1LL * height * dsu.size[a];

            if (dsu.size[a] < dsu.size[b]) swap(a, b);
            dsu.parent[b] = a;
            dsu.size[a] += dsu.size[b];
            dsu.node[a] = merged;
        }

        // Every parent was created after its children.
        for (int v = nodes - 1; v >= 0; --v) {
            if (tree_parent[v] != -1) {
                contribution[v] += contribution[tree_parent[v]];
            }
        }
        for (int i = 0; i < n; ++i) {
            cout << contribution[i] << (i + 1 == n ? '\n' : ' ');
        }
    }
}
