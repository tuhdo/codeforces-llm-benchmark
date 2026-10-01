#include <bits/stdc++.h>
using namespace std;

struct TwoSAT {
    int n;
    vector<vector<int>> g, rg;

    explicit TwoSAT(int n) : n(n), g(2 * n), rg(2 * n) {}

    static int neg(int literal) { return literal ^ 1; }

    void implication(int from, int to) {
        g[from].push_back(to);
        rg[to].push_back(from);
    }

    void add_clause(int a, int b) {
        implication(neg(a), b);
        implication(neg(b), a);
    }

    void add_true(int variable) {
        add_clause(2 * variable + 1, 2 * variable + 1);
    }

    void add_false(int variable) {
        add_clause(2 * variable, 2 * variable);
    }

    bool solve(vector<int>& value) {
        const int vertices = 2 * n;
        vector<char> seen(vertices, false);
        vector<int> order;
        order.reserve(vertices);

        for (int start = 0; start < vertices; ++start) {
            if (seen[start]) continue;
            vector<pair<int, int>> stack;
            stack.push_back({start, 0});
            seen[start] = true;
            while (!stack.empty()) {
                int v = stack.back().first;
                int& edge = stack.back().second;
                if (edge < (int)g[v].size()) {
                    int to = g[v][edge++];
                    if (!seen[to]) {
                        seen[to] = true;
                        stack.push_back({to, 0});
                    }
                } else {
                    order.push_back(v);
                    stack.pop_back();
                }
            }
        }

        vector<int> component(vertices, -1);
        int components = 0;
        for (int oi = vertices - 1; oi >= 0; --oi) {
            int start = order[oi];
            if (component[start] != -1) continue;
            vector<int> stack = {start};
            component[start] = components;
            while (!stack.empty()) {
                int v = stack.back();
                stack.pop_back();
                for (int to : rg[v]) {
                    if (component[to] == -1) {
                        component[to] = components;
                        stack.push_back(to);
                    }
                }
            }
            ++components;
        }

        value.assign(n, 0);
        for (int i = 0; i < n; ++i) {
            if (component[2 * i] == component[2 * i + 1]) return false;
            value[i] = component[2 * i + 1] > component[2 * i];
        }
        return true;
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
        vector<int> b(n);
        for (int& x : b) cin >> x;

        TwoSAT sat(n);
        vector<int> difference(n + 1, 0);

        auto forbid_range = [&](int left, int right) {
            left = max(left, 0);
            right = min(right, n - 1);
            if (left <= right) {
                ++difference[left];
                --difference[right + 1];
            }
        };

        for (int i = 0; i < n; ++i) {
            if (b[i] == -1) continue;
            if (b[i] == 0) {
                sat.add_true(i);
                continue;
            }

            int d = b[i];
            forbid_range(i - d + 1, i + d - 1);

            bool has_endpoint = false;
            if (0 <= i - d) {
                has_endpoint = true;
            }
            if (i + d < n) {
                has_endpoint = true;
            }
            if (!has_endpoint) {
                sat.add_clause(0, 0); // Contradiction: false OR false.
                continue;
            }

            if (0 <= i - d && i + d < n) {
                sat.add_clause(2 * (i - d) + 1, 2 * (i + d) + 1);
            } else if (0 <= i - d) {
                sat.add_true(i - d);
            } else {
                sat.add_true(i + d);
            }
        }

        int active = 0;
        for (int i = 0; i < n; ++i) {
            active += difference[i];
            if (active > 0) sat.add_false(i);
        }

        vector<int> answer;
        if (!sat.solve(answer)) {
            cout << "-1\n";
            continue;
        }

        bool any = false;
        for (int x : answer) any |= (x != 0);
        if (!any) answer[0] = 1; // This can only happen when every entry was destroyed.

        for (int x : answer) cout << x;
        cout << '\n';
    }
}
