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
        for (int &x : a) cin >> x;
        for (int &x : b) cin >> x;

        bool possible = true;
        for (int i = 0; i < n; ++i) {
            if (a[i] > b[i]) possible = false;
        }

        vector<int> active;
        for (int i = 0; i < n; ++i) {
            if (a[i] < b[i]) active.push_back(i);
        }

        if (possible && !active.empty()) {
            auto root = [&](int v) {
                while (b[v] != v + 1) v = b[v] - 1;
                return v;
            };
            int r = root(active[0]);
            for (int v : active) {
                if (root(v) != r) {
                    possible = false;
                    break;
                }
            }
        }

        vector<int> answer;
        int start = active.empty() ? 0 : active[0];

        if (possible && !active.empty()) {
            vector<int> cur = a;
            vector<char> done(n, false);
            int p = start;

            for (int finished = 0; finished < (int)active.size(); ++finished) {
                vector<int> parent(n, -1);
                set<int> unseen;
                for (int i = 0; i < n; ++i) unseen.insert(i);
                unseen.erase(p);

                queue<int> q;
                q.push(p);
                parent[p] = p;

                while (!q.empty()) {
                    int u = q.front();
                    q.pop();

                    auto it = unseen.lower_bound(cur[u] - 1);
                    while (it != unseen.end() && *it < b[u]) {
                        int v = *it;
                        parent[v] = u;
                        q.push(v);
                        it = unseen.erase(it);
                    }
                }

                int target = -1;
                for (int v : active) {
                    if (!done[v] && parent[v] != -1) {
                        target = v;
                        break;
                    }
                }

                if (target == -1) {
                    possible = false;
                    break;
                }

                vector<int> path;
                for (int v = target;; v = parent[v]) {
                    path.push_back(v);
                    if (v == p) break;
                }
                reverse(path.begin(), path.end());

                for (int i = 0; i + 1 < (int)path.size(); ++i) {
                    int u = path[i];
                    int v = path[i + 1];
                    while (cur[u] < v + 1) {
                        ++cur[u];
                        answer.push_back(1);
                    }
                    answer.push_back(2);
                    p = v;
                }

                while (cur[target] < b[target]) {
                    ++cur[target];
                    answer.push_back(1);
                }
                done[target] = true;
                p = cur[target] - 1;
                answer.push_back(2);
            }

            if (possible) {
                for (int i = 0; i < n; ++i) {
                    if (cur[i] != b[i]) possible = false;
                }
            }
        }

        if (!possible) {
            cout << -1 << '\n';
            continue;
        }

        if (active.empty()) {
            cout << 0 << ' ' << 1 << '\n' << '\n';
            continue;
        }

        cout << answer.size() << ' ' << start + 1 << '\n';
        for (int x : answer) cout << x << ' ';
        cout << '\n';
    }
    return 0;
}
