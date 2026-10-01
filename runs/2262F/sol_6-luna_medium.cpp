#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n, m;
        cin >> n >> m;
        vector<pair<int,int>> cells(m);
        vector<vector<int>> adj(n);
        for (auto &[x, y] : cells) {
            cin >> x >> y;
            --x; --y;
            adj[x].push_back(y);
        }

        // A nonsingular binary matrix has a nonzero determinant term, hence
        // its support contains a perfect matching.
        vector<int> row_for_col(n, -1);
        function<bool(int, vector<char>&)> augment = [&](int row, vector<char>& seen) {
            for (int col : adj[row]) {
                if (seen[col]) continue;
                seen[col] = true;
                if (row_for_col[col] == -1 || augment(row_for_col[col], seen)) {
                    row_for_col[col] = row;
                    return true;
                }
            }
            return false;
        };
        for (int row = 0; row < n; ++row) {
            vector<char> seen(n, false);
            augment(row, seen);
        }

        vector<pair<int,int>> matching;
        vector<char> is_matching(m, false);
        // Mark matching cells using the input list; input cells are unique.
        vector<int> matched_row_col(n, -1);
        for (int col = 0; col < n; ++col)
            matched_row_col[row_for_col[col]] = col;
        for (int i = 0; i < m; ++i) {
            auto [x, y] = cells[i];
            if (matched_row_col[x] == y) {
                is_matching[i] = true;
                matching.push_back(cells[i]);
            }
        }

        vector<pair<int,int>> extra;
        for (int i = 0; i < m; ++i)
            if (!is_matching[i]) extra.push_back(cells[i]);

        int q = m / n, r = m % n;
        int k = (m + n - 1) / n;
        cout << k << '\n';
        int ptr = 0;

        // Remove n non-matching cells in each of the first q-1 moves.
        for (int move = 0; move < q - 1; ++move) {
            cout << n << '\n';
            for (int j = 0; j < n; ++j) {
                auto [x, y] = extra[ptr++];
                cout << x + 1 << ' ' << y + 1 << '\n';
            }
        }

        if (r == 0) {
            // The remaining cells are exactly the perfect matching.
            cout << n << '\n';
            for (auto [x, y] : matching) cout << x + 1 << ' ' << y + 1 << '\n';
        } else {
            // One rank-n move leaves r matching cells, then remove those r.
            cout << n << '\n';
            for (; ptr < (int)extra.size(); ++ptr) {
                auto [x, y] = extra[ptr];
                cout << x + 1 << ' ' << y + 1 << '\n';
            }
            for (int i = 0; i < n - r; ++i)
                cout << matching[i].first + 1 << ' ' << matching[i].second + 1 << '\n';
            cout << r << '\n';
            for (int i = n - r; i < n; ++i)
                cout << matching[i].first + 1 << ' ' << matching[i].second + 1 << '\n';
        }
    }
    return 0;
}
