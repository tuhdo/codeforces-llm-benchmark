#include <bits/stdc++.h>
#include <cassert>
using namespace std;

struct Cell {
    int row, col;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCount;
    cin >> testCount;
    while (testCount--) {
        int n, m;
        cin >> n >> m;

        vector<Cell> cells(m);
        vector<vector<pair<int, int>>> graph(n);
        for (int i = 0; i < m; ++i) {
            cin >> cells[i].row >> cells[i].col;
            --cells[i].row;
            --cells[i].col;
            graph[cells[i].row].push_back({cells[i].col, i});
        }

        // A full-rank binary matrix has a perfect matching in its support.
        vector<int> matchedColumn(n, -1);
        auto augment = [&](auto&& self, int row, vector<char>& seen) -> bool {
            for (auto [col, edge] : graph[row]) {
                if (seen[col]) continue;
                seen[col] = true;
                if (matchedColumn[col] == -1 || self(self, matchedColumn[col], seen)) {
                    matchedColumn[col] = row;
                    return true;
                }
            }
            return false;
        };

        for (int row = 0; row < n; ++row) {
            vector<char> seen(n);
            bool ok = augment(augment, row, seen);
            assert(ok);
        }

        vector<char> inMatching(m);
        vector<int> ownerOfColumn(n);
        for (int col = 0; col < n; ++col) {
            int row = matchedColumn[col];
            ownerOfColumn[col] = row;
            for (auto [to, edge] : graph[row]) {
                if (to == col) {
                    inMatching[edge] = true;
                    break;
                }
            }
        }

        vector<Cell> matching, extra;
        for (int i = 0; i < m; ++i) {
            (inMatching[i] ? matching : extra).push_back(cells[i]);
        }

        int moves = (m + n - 1) / n;
        int lastSize = m - (moves - 1) * n;

        // If there will be an earlier move, retain lastSize extra cells that
        // are all above or all below the matching diagonal.  After columns
        // are permuted according to the matching, these cells form a
        // strictly triangular matrix, so matching + retained cells is still
        // invertible.
        if (moves >= 3) {
            int forward = 0;
            for (const Cell& cell : extra) {
                forward += cell.row < ownerOfColumn[cell.col];
            }
            bool keepForward = forward * 2 >= (int)extra.size();
            vector<Cell> removable, kept;
            for (const Cell& cell : extra) {
                bool isForward = cell.row < ownerOfColumn[cell.col];
                if (isForward == keepForward && (int)kept.size() < lastSize) {
                    kept.push_back(cell);
                } else {
                    removable.push_back(cell);
                }
            }
            // There are at least n + lastSize extra cells here, so one of
            // the two directions contains at least lastSize of them.
            assert((int)kept.size() == lastSize);
            extra = move(removable);
            extra.insert(extra.end(), kept.begin(), kept.end());
        }
        vector<vector<Cell>> answer;

        // Reserve lastSize non-matching cells for the penultimate move.
        // Each earlier move deletes n other non-matching cells, so the full
        // perfect matching remains and the rank stays n.
        int removableExtras = (int)extra.size() - lastSize;
        for (int start = 0; start < removableExtras; start += n) {
            vector<Cell> move;
            for (int j = 0; j < n; ++j) move.push_back(extra[start + j]);
            answer.push_back(move);
        }

        if (moves == 1) {
            answer.push_back(matching);
        } else {
            vector<Cell> penultimate;
            for (int i = lastSize; i < n; ++i) penultimate.push_back(matching[i]);
            for (int i = removableExtras; i < (int)extra.size(); ++i) {
                penultimate.push_back(extra[i]);
            }
            assert((int)penultimate.size() == n);
            answer.push_back(penultimate);

            vector<Cell> last(matching.begin(), matching.begin() + lastSize);
            answer.push_back(last);
        }

        assert((int)answer.size() == moves);
        cout << moves << '\n';
        for (const auto& move : answer) {
            cout << move.size() << '\n';
            for (const Cell& cell : move) {
                cout << cell.row + 1 << ' ' << cell.col + 1 << '\n';
            }
        }
    }
}
