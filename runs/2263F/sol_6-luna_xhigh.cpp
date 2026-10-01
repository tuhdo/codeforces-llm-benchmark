#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
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

        int changed = 0;
        for (int i = 0; i < n; ++i) {
            changed += (a[i] != b[i]);
        }
        if (changed == 0) {
            cout << "0 1\n\n";
            continue;
        }

        bool possible = true;
        for (int i = 0; i < n; ++i) {
            if (a[i] > b[i]) possible = false;
        }
        if (!possible) {
            cout << -1 << '\n';
            continue;
        }

        // Mark all vertices on target-road paths from vertices that change.
        vector<char> useful(n, false);
        for (int i = 0; i < n; ++i) {
            if (a[i] == b[i]) continue;
            int v = i;
            while (!useful[v]) {
                useful[v] = true;
                v = b[v];
            }
        }

        vector<int> ends;
        for (int i = 0; i < n; ++i) {
            if (useful[i] && b[i] == i) ends.push_back(i);
        }
        if (ends.size() != 1) {
            cout << -1 << '\n';
            continue;
        }

        // From i, every vertex in [a[i], b[i]] can be selected as the
        // destination by applying operation 1 the appropriate number of times.
        vector<vector<int>> graph(n), reverseGraph(n);
        for (int i = 0; i < n; ++i) {
            for (int j = a[i]; j <= b[i]; ++j) {
                graph[i].push_back(j);
                reverseGraph[j].push_back(i);
            }
        }

        // Tarjan SCC decomposition of the interval-transition graph.
        vector<int> discovery(n, -1), low(n), component(n, -1), stack;
        int timer = 0;
        function<void(int)> dfs = [&](int v) {
            discovery[v] = low[v] = timer++;
            stack.push_back(v);

            for (int u : graph[v]) {
                if (discovery[u] == -1) {
                    dfs(u);
                    low[v] = min(low[v], low[u]);
                } else if (component[u] == -1) {
                    low[v] = min(low[v], discovery[u]);
                }
            }

            if (low[v] == discovery[v]) {
                while (true) {
                    int u = stack.back();
                    stack.pop_back();
                    component[u] = v;
                    if (u == v) break;
                }
            }
        };

        for (int i = 0; i < n; ++i) {
            if (discovery[i] == -1) dfs(i);
        }

        vector<vector<int>> members(n);
        for (int i = 0; i < n; ++i) members[component[i]].push_back(i);

        // Target roads between SCCs must form a single chain into the unique
        // terminal SCC, and all changing vertices must be on that chain.
        vector<int> predecessor(n, -1), changingInComponent(n, 0);
        bool chainPossible = true;
        for (int i = 0; i < n; ++i) {
            if (a[i] != b[i]) ++changingInComponent[component[i]];
            if (useful[i] && component[i] != component[b[i]]) {
                int destination = component[b[i]];
                if (predecessor[destination] != -1) {
                    chainPossible = false;
                } else {
                    predecessor[destination] = component[i];
                }
            }
        }

        vector<int> chain;
        int currentComponent = component[ends[0]];
        int chainChanges = 0;
        while (currentComponent != -1) {
            chain.push_back(currentComponent);
            chainChanges += changingInComponent[currentComponent];
            currentComponent = predecessor[currentComponent];
        }
        if (chainChanges != changed) chainPossible = false;
        if (!chainPossible) {
            cout << -1 << '\n';
            continue;
        }
        reverse(chain.begin(), chain.end());

        int position = members[chain[0]][0];
        const int start = position;
        vector<int> operations;

        auto increment = [&]() {
            operations.push_back(1);
            ++a[position];
        };
        auto follow = [&]() {
            operations.push_back(2);
            position = a[position];
        };

        for (int root : chain) {
            const vector<int> &group = members[root];
            vector<int> nextInTree(n, -1);

            // Pick a spanning tree inside this SCC, directed toward group[0].
            function<void(int)> buildTree = [&](int v) {
                for (int u : reverseGraph[v]) {
                    if (component[u] == root && nextInTree[u] == -1) {
                        nextInTree[u] = v;
                        buildTree(u);
                    }
                }
            };
            buildTree(group[0]);

            for (int target : group) {
                while (position != target) {
                    while (a[position] < max(nextInTree[position], target)) {
                        increment();
                    }
                    follow();
                }
                while (a[position] < b[position]) increment();
                follow();
            }
        }

        cout << operations.size() << ' ' << start + 1 << '\n';
        for (int op : operations) cout << op << ' ';
        cout << '\n';
    }
    return 0;
}
