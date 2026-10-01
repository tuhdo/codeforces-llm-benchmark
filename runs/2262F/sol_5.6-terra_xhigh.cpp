#include <bits/stdc++.h>
#include <cassert>
using namespace std;

struct Cell {
    int row;
    int col;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int test_count;
    cin >> test_count;

    while (test_count--) {
        int n, m;
        cin >> n >> m;

        vector<Cell> cells(m);
        vector<vector<int>> edges(n);
        for (int i = 0; i < m; ++i) {
            cin >> cells[i].row >> cells[i].col;
            --cells[i].row;
            --cells[i].col;
            edges[cells[i].row].push_back(cells[i].col);
        }

        // A full-rank matrix has a perfect matching in its bipartite graph.
        vector<int> matched_row_for_col(n, -1);
        auto augment = [&](auto&& self, int row, vector<char>& seen) -> bool {
            for (int col : edges[row]) {
                if (seen[col]) {
                    continue;
                }
                seen[col] = true;
                if (matched_row_for_col[col] == -1 ||
                    self(self, matched_row_for_col[col], seen)) {
                    matched_row_for_col[col] = row;
                    return true;
                }
            }
            return false;
        };

        for (int row = 0; row < n; ++row) {
            vector<char> seen(n, false);
            bool found = augment(augment, row, seen);
            assert(found);
        }

        vector<int> matched_col_for_row(n);
        for (int col = 0; col < n; ++col) {
            matched_col_for_row[matched_row_for_col[col]] = col;
        }

        vector<vector<char>> is_matched(n, vector<char>(n, false));
        for (int row = 0; row < n; ++row) {
            is_matched[row][matched_col_for_row[row]] = true;
        }

        vector<Cell> non_matching;
        for (const Cell& cell : cells) {
            if (!is_matched[cell.row][cell.col]) {
                non_matching.push_back(cell);
            }
        }

        const int remainder = m % n;
        vector<vector<Cell>> moves;

        if (remainder == 0) {
            // Delete all non-matching cells first.  The matching preserves rank n.
            for (int start = 0; start < (int)non_matching.size(); start += n) {
                moves.emplace_back(non_matching.begin() + start,
                                   non_matching.begin() + start + n);
            }

            vector<Cell> matching;
            for (int row = 0; row < n; ++row) {
                matching.push_back({row, matched_col_for_row[row]});
            }
            moves.push_back(move(matching));
        } else {
            // Keep `remainder` matching cells for the final move, and use the
            // same number of non-matching cells in the preceding rank-n move.
            vector<Cell> transition_cells(non_matching.begin(),
                                          non_matching.begin() + remainder);

            for (int start = remainder; start < (int)non_matching.size(); start += n) {
                moves.emplace_back(non_matching.begin() + start,
                                   non_matching.begin() + start + n);
            }

            for (int row = remainder; row < n; ++row) {
                transition_cells.push_back({row, matched_col_for_row[row]});
            }
            moves.push_back(move(transition_cells));

            vector<Cell> last_move;
            for (int row = 0; row < remainder; ++row) {
                last_move.push_back({row, matched_col_for_row[row]});
            }
            moves.push_back(move(last_move));
        }

        cout << moves.size() << '\n';
        for (const auto& current_move : moves) {
            cout << current_move.size() << '\n';
            for (const Cell& cell : current_move) {
                cout << cell.row + 1 << ' ' << cell.col + 1 << '\n';
            }
        }
    }

    return 0;
}
