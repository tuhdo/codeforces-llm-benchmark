#include <bits/stdc++.h>
using namespace std;

struct SuccessorSet {
    vector<int> parent;
    explicit SuccessorSet(int n) : parent(n + 2) {
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        int root = x;
        while (parent[root] != root) root = parent[root];
        while (parent[x] != x) {
            int next = parent[x];
            parent[x] = root;
            x = next;
        }
        return root;
    }
    void erase(int x) { parent[x] = find(x + 1); }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<int> a(n + 1), b(n + 1);
        for (int i = 1; i <= n; ++i) cin >> a[i];
        for (int i = 1; i <= n; ++i) cin >> b[i];

        bool possible = true;
        int start = 1;
        for (int i = n; i >= 1; --i) {
            if (a[i] > b[i]) possible = false;
            if (a[i] != b[i]) start = i;
        }
        int position = start;
        vector<int> operations;
        operations.reserve(2 * n * n);

        for (int target = 1; target <= n && possible; ++target) {
            if (a[target] == b[target]) continue;

            vector<int> previous(n + 1, -1), queue;
            queue.reserve(n);
            SuccessorSet unvisited(n);
            previous[position] = position;
            queue.push_back(position);
            unvisited.erase(position);

            for (int head = 0; head < (int)queue.size() && previous[target] == -1; ++head) {
                int u = queue[head];
                for (int v = unvisited.find(a[u]); v <= b[u]; v = unvisited.find(v)) {
                    previous[v] = u;
                    queue.push_back(v);
                    unvisited.erase(v);
                }
            }
            if (previous[target] == -1) {
                possible = false;
                break;
            }

            vector<int> path;
            for (int v = target; v != position; v = previous[v]) path.push_back(v);
            reverse(path.begin(), path.end());
            for (int v : path) {
                while (a[position] < v) {
                    ++a[position];
                    operations.push_back(1);
                }
                operations.push_back(2);
                position = v;
            }
            while (a[position] < b[position]) {
                ++a[position];
                operations.push_back(1);
            }
        }

        if (!possible) {
            cout << -1 << '\n';
        } else {
            cout << operations.size() << ' ' << start << '\n';
            for (int operation : operations) cout << operation << ' ';
            cout << '\n';
        }
    }
}
