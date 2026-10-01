#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n, m;
        cin >> n >> m;

        vector<pair<int, int>> cells;
        vector<vector<int>> adj(n);
        for (int i = 0; i < m; ++i) {
            int x, y;
            cin >> x >> y;
            --x;
            --y;
            cells.push_back({x, y});
            adj[x].push_back(y);
        }

        // Find a perfect matching in the support of the matrix.
        vector<int> matched_row(n, -1);
        function<bool(int, vector<int>&)> augment = [&](int row, vector<int>& seen) {
            for (int col : adj[row]) {
                if (seen[col]) continue;
                seen[col] = 1;
                if (matched_row[col] == -1 || augment(matched_row[col], seen)) {
                    matched_row[col] = row;
                    return true;
                }
            }
            return false;
        };

        for (int row = 0; row < n; ++row) {
            vector<int> seen(n, 0);
            bool ok = augment(row, seen);
            (void)ok; // Existence is guaranteed by rank(M) = n.
        }

        vector<pair<int, int>> basis;
        vector<vector<char>> is_basis(n, vector<char>(n, 0));
        for (int col = 0; col < n; ++col) {
            int row = matched_row[col];
            basis.push_back({row, col});
            is_basis[row][col] = 1;
        }

        vector<pair<int, int>> extra;
        for (auto cell : cells) {
            if (!is_basis[cell.first][cell.second]) extra.push_back(cell);
        }

        int moves = (m + n - 1) / n;
        int q = m - (moves - 1) * n; // Number of cells left for the last move.
        vector<vector<pair<int, int>>> answer;

        int pos = 0;
        for (int move = 0; move < moves - 2; ++move) {
            vector<pair<int, int>> current;
            for (int j = 0; j < n; ++j) current.push_back(extra[pos++]);
            answer.push_back(current);
        }

        // Remove the remaining q extra cells and n-q basis cells.
        if (moves >= 2) {
            vector<pair<int, int>> current;
            while (pos < (int)extra.size()) current.push_back(extra[pos++]);
            for (int i = q; i < n; ++i) current.push_back(basis[i]);
            answer.push_back(current);
        }

        // The remaining q basis cells have rank q and are removed together.
        vector<pair<int, int>> last;
        for (int i = 0; i < q; ++i) last.push_back(basis[i]);
        answer.push_back(move(move(last)));

        cout << answer.size() << '\n';
        for (const auto& move_cells : answer) {
            cout << move_cells.size() << '\n';
            for (auto [x, y] : move_cells) cout << x + 1 << ' ' << y + 1 << '\n';
        }
    }
    return 0;
}
