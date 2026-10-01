#include <bits/stdc++.h>
using namespace std;

struct Cell {
    int x, y;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n, m;
        cin >> n >> m;
        vector<Cell> cells(m);
        vector<vector<int>> adj(n);
        for (int i = 0; i < m; ++i) {
            cin >> cells[i].x >> cells[i].y;
            --cells[i].x;
            --cells[i].y;
            adj[cells[i].x].push_back(cells[i].y);
        }

        // A full-rank matrix has nonzero determinant, so its support
        // contains a perfect matching.
        vector<int> match_col(n, -1);
        function<bool(int, vector<char>&)> augment = [&](int row, vector<char>& seen) {
            for (int col : adj[row]) {
                if (seen[col]) continue;
                seen[col] = true;
                if (match_col[col] == -1 || augment(match_col[col], seen)) {
                    match_col[col] = row;
                    return true;
                }
            }
            return false;
        };
        for (int row = 0; row < n; ++row) {
            vector<char> seen(n, false);
            augment(row, seen);
        }

        vector<int> matching_cell;
        vector<char> is_matching(m, false);
        vector<int> row_to_col(n, -1);
        for (int col = 0; col < n; ++col) row_to_col[match_col[col]] = col;
        for (int i = 0; i < m; ++i) {
            if (cells[i].y == row_to_col[cells[i].x]) {
                matching_cell.push_back(i);
                is_matching[i] = true;
            }
        }

        int q = m / n, rem = m % n;
        vector<int> q_cells;
        for (int i = 0; i < rem; ++i) q_cells.push_back(matching_cell[i]);
        vector<char> is_q(m, false);
        for (int id : q_cells) is_q[id] = true;

        vector<int> reserved;
        if (rem > 0) {
            for (int i = 0; i < m && (int)reserved.size() < rem; ++i) {
                if (!is_matching[i]) reserved.push_back(i);
            }
        }
        vector<char> is_reserved(m, false);
        for (int id : reserved) is_reserved[id] = true;

        vector<vector<int>> moves;
        vector<int> removable;
        for (int i = 0; i < m; ++i) {
            if (!is_matching[i] && !is_reserved[i]) removable.push_back(i);
        }
        for (int at = 0; at < (int)removable.size(); at += n) {
            vector<int> move;
            for (int j = at; j < at + n; ++j) move.push_back(removable[j]);
            moves.push_back(move);
        }

        if (rem == 0) {
            moves.push_back(matching_cell);
        } else {
            vector<int> penultimate;
            for (int id : matching_cell) if (!is_q[id]) penultimate.push_back(id);
            for (int id : reserved) penultimate.push_back(id);
            moves.push_back(penultimate);
            moves.push_back(q_cells);
        }

        cout << moves.size() << '\n';
        for (const auto& move : moves) {
            cout << move.size() << '\n';
            for (int id : move)
                cout << cells[id].x + 1 << ' ' << cells[id].y + 1 << '\n';
        }
    }
    return 0;
}
