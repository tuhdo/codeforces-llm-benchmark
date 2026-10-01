#include <bits/stdc++.h>
using namespace std;

struct Cell {
    int row;
    int col;
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
            cin >> cells[i].row >> cells[i].col;
            --cells[i].row;
            --cells[i].col;
            adj[cells[i].row].push_back(cells[i].col);
        }

        vector<int> matchedRow(n, -1);
        function<bool(int, vector<char>&)> augment = [&](int row, vector<char>& seen) {
            for (int col : adj[row]) {
                if (seen[col]) continue;
                seen[col] = true;
                if (matchedRow[col] == -1 || augment(matchedRow[col], seen)) {
                    matchedRow[col] = row;
                    return true;
                }
            }
            return false;
        };

        for (int row = 0; row < n; ++row) {
            vector<char> seen(n, false);
            augment(row, seen);
        }

        vector<int> matchedCol(n, -1);
        for (int col = 0; col < n; ++col) {
            if (matchedRow[col] != -1) matchedCol[matchedRow[col]] = col;
        }

        vector<char> isMatchingCell(m, false);
        vector<int> matchingIndices;
        matchingIndices.reserve(n);
        for (int i = 0; i < m; ++i) {
            if (matchedCol[cells[i].row] == cells[i].col) {
                isMatchingCell[i] = true;
                matchingIndices.push_back(i);
            }
        }

        int moves = (m + n - 1) / n;
        int finalRank = m - (moves - 1) * n;
        vector<vector<Cell>> plan;
        plan.reserve(moves);

        if (moves == 1) {
            plan.push_back(cells);
        } else {
            vector<int> extraIndices;
            extraIndices.reserve(m - n);
            for (int i = 0; i < m; ++i) {
                if (!isMatchingCell[i]) extraIndices.push_back(i);
            }

            vector<char> removed(m, false);
            size_t extraPos = 0;
            for (int step = 0; step < moves - 2; ++step) {
                vector<Cell> move;
                move.reserve(n);
                for (int j = 0; j < n; ++j) {
                    int index = extraIndices[extraPos++];
                    removed[index] = true;
                    move.push_back(cells[index]);
                }
                plan.push_back(move);
            }

            vector<char> keep(m, false);
            for (int i = 0; i < finalRank; ++i) {
                keep[matchingIndices[i]] = true;
            }

            vector<Cell> penultimate;
            penultimate.reserve(n);
            for (int i = 0; i < m; ++i) {
                if (!removed[i] && !keep[i]) penultimate.push_back(cells[i]);
            }
            plan.push_back(penultimate);

            vector<Cell> last;
            last.reserve(finalRank);
            for (int i = 0; i < m; ++i) {
                if (keep[i]) last.push_back(cells[i]);
            }
            plan.push_back(last);
        }

        cout << moves << '\n';
        for (const auto& move : plan) {
            cout << move.size() << '\n';
            for (const Cell& cell : move) {
                cout << cell.row + 1 << ' ' << cell.col + 1 << '\n';
            }
        }
    }
    return 0;
}
