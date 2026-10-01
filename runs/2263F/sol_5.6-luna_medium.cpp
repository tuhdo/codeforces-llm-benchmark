#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        vector<int> a(n), b(n);
        for (int &x : a) {
            cin >> x;
            --x;
        }
        for (int &x : b) {
            cin >> x;
            --x;
        }

        int different = 0;
        bool possible = true;
        for (int i = 0; i < n; ++i) {
            if (a[i] > b[i]) possible = false;
            different += (a[i] != b[i]);
        }
        if (!possible) {
            cout << "-1\n";
            continue;
        }
        if (different == 0) {
            cout << "0 1\n\n";
            continue;
        }

        // Vertices that must be visited, together with all vertices on their
        // final b-paths.
        vector<char> useful(n, false);
        for (int i = 0; i < n; ++i) {
            if (a[i] == b[i]) continue;
            int x = i;
            while (!useful[x]) {
                useful[x] = true;
                x = b[x];
            }
        }

        vector<int> ends;
        for (int i = 0; i < n; ++i)
            if (useful[i] && b[i] == i) ends.push_back(i);
        if (ends.size() > 1) {
            cout << "-1\n";
            continue;
        }

        vector<vector<int>> g(n), rg(n);
        for (int i = 0; i < n; ++i) {
            for (int j = a[i]; j <= b[i]; ++j) {
                g[i].push_back(j);
                rg[j].push_back(i);
            }
        }

        // Tarjan SCC decomposition.
        vector<int> tin(n, -1), low(n), stack_vertices;
        vector<int> root(n, -1);
        int timer = 0;
        function<void(int)> dfs = [&](int v) {
            tin[v] = low[v] = timer++;
            stack_vertices.push_back(v);
            for (int u : g[v]) {
                if (tin[u] == -1) {
                    dfs(u);
                    low[v] = min(low[v], low[u]);
                } else if (root[u] == -1) {
                    low[v] = min(low[v], tin[u]);
                }
            }
            if (low[v] == tin[v]) {
                while (true) {
                    int u = stack_vertices.back();
                    stack_vertices.pop_back();
                    root[u] = v;
                    if (u == v) break;
                }
            }
        };
        for (int i = 0; i < n; ++i)
            if (tin[i] == -1) dfs(i);

        vector<vector<int>> components(n);
        for (int i = 0; i < n; ++i) components[root[i]].push_back(i);

        // The useful SCCs must form one directed chain.
        vector<int> parent(n, -1), count_changed(n, 0);
        bool chain_ok = true;
        for (int i = 0; i < n; ++i) {
            count_changed[root[i]] += (a[i] != b[i]);
            if (useful[i] && root[i] != root[b[i]]) {
                int &p = parent[root[b[i]]];
                if (p != -1 && p != root[i]) chain_ok = false;
                p = root[i];
            }
        }
        if (!chain_ok) {
            cout << "-1\n";
            continue;
        }

        vector<int> chain;
        for (int x = root[ends[0]]; x != -1; x = parent[x])
            chain.push_back(x);
        int chain_changed = 0;
        for (int c : chain) chain_changed += count_changed[c];
        if (chain_changed != different) {
            cout << "-1\n";
            continue;
        }
        reverse(chain.begin(), chain.end());

        int start = components[chain[0]][0];
        int position = start;
        vector<int> operations;

        auto increase = [&]() {
            // Every call is made only while a[position] < b[position] <= n-1.
            ++a[position];
            operations.push_back(1);
        };
        auto follow = [&]() {
            operations.push_back(2);
            position = a[position];
        };

        for (int component_root : chain) {
            vector<int> &component = components[component_root];

            // Build a reverse spanning tree of this SCC, rooted at the first
            // vertex.  It gives a route to every vertex in the component.
            vector<int> next_in_tree(n, -1);
            function<void(int)> build_tree = [&](int v) {
                for (int u : rg[v]) {
                    if (root[u] != component_root || next_in_tree[u] != -1)
                        continue;
                    next_in_tree[u] = v;
                    build_tree(u);
                }
            };
            build_tree(component[0]);

            for (int target : component) {
                while (position != target) {
                    while (a[position] < max(next_in_tree[position], target))
                        increase();
                    follow();
                }
                while (a[position] < b[position]) increase();
                // Leave through the final b-edge.  A self-loop at the very
                // end is harmless and keeps the construction uniform.
                follow();
            }
        }

        // The generic traversal above can finish at a target without needing
        // to follow its final edge.  It must nevertheless have processed all
        // vertices; the following assertion is only a development invariant.
        bool valid = true;
        for (int i = 0; i < n; ++i) valid &= (a[i] == b[i]);
        if (!valid || operations.size() > 2LL * n * n) {
            cout << "-1\n";
            continue;
        }

        cout << operations.size() << ' ' << start + 1 << '\n';
        for (int op : operations) cout << op << ' ';
        cout << '\n';
    }
}
