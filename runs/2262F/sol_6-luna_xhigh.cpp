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

        vector<pair<int, int>> cells(m);
        vector<vector<int>> graph(n);
        vector<vector<int>> cell_id(n, vector<int>(n, -1));
        for (int i = 0; i < m; ++i) {
            int x, y;
            cin >> x >> y;
            --x;
            --y;
            cells[i] = {x, y};
            graph[x].push_back(y);
            cell_id[x][y] = i;
        }

        // A full-rank binary matrix has a nonzero determinant term, hence
        // its 1-cells contain a perfect matching.
        vector<int> matched_row_for_col(n, -1);
        function<bool(int, vector<char>&)> augment = [&](int row, vector<char>& seen) {
            for (int col : graph[row]) {
                if (seen[col]) continue;
                seen[col] = true;
                if (matched_row_for_col[col] == -1 ||
                    augment(matched_row_for_col[col], seen)) {
                    matched_row_for_col[col] = row;
                    return true;
                }
            }
            return false;
        };

        for (int row = 0; row < n; ++row) {
            vector<char> seen(n, false);
            augment(row, seen);
        }

        vector<int> matched_col_for_row(n, -1);
        for (int col = 0; col < n; ++col) {
            if (matched_row_for_col[col] != -1) {
                matched_col_for_row[matched_row_for_col[col]] = col;
            }
        }

        vector<int> basis;
        vector<char> is_basis(m, false);
        for (int row = 0; row < n; ++row) {
            int id = cell_id[row][matched_col_for_row[row]];
            basis.push_back(id);
            is_basis[id] = true;
        }

        vector<int> extra;
        for (int i = 0; i < m; ++i) {
            if (!is_basis[i]) extra.push_back(i);
        }

        vector<vector<int>> moves;
        size_t pos = 0;

        // Remove batches of n non-matching cells while keeping the matching.
        while (extra.size() - pos >= static_cast<size_t>(n)) {
            vector<int> move;
            for (int j = 0; j < n; ++j) move.push_back(extra[pos++]);
            moves.push_back(move);
        }

        int remaining = static_cast<int>(extra.size() - pos);
        if (remaining == 0) {
            moves.push_back(basis);
        } else {
            // Leave 'remaining' matching cells, so the last move has exactly
            // the rank of the remaining matrix.
            vector<int> move;
            for (size_t j = pos; j < extra.size(); ++j) move.push_back(extra[j]);
            for (int j = remaining; j < n; ++j) move.push_back(basis[j]);
            moves.push_back(move);

            vector<int> last_move;
            for (int j = 0; j < remaining; ++j) last_move.push_back(basis[j]);
            moves.push_back(last_move);
        }

        cout << moves.size() << '\n';
        for (const auto& move : moves) {
            cout << move.size() << '\n';
            for (int id : move) {
                cout << cells[id].first + 1 << ' ' << cells[id].second + 1 << '\n';
            }
        }
    }

    return 0;
}
