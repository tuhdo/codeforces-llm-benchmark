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

        vector<vector<int>> adj(n);
        vector<pair<int, int>> cells;
        vector<vector<bool>> in_matching(n, vector<bool>(n, false));
        for (int i = 0; i < m; ++i) {
            int x, y;
            cin >> x >> y;
            --x;
            --y;
            adj[x].push_back(y);
            cells.push_back({x, y});
        }

        vector<int> match_right(n, -1);
        function<bool(int, vector<bool>&)> augment = [&](int row, vector<bool>& seen) {
            for (int col : adj[row]) {
                if (seen[col]) continue;
                seen[col] = true;
                if (match_right[col] == -1 || augment(match_right[col], seen)) {
                    match_right[col] = row;
                    return true;
                }
            }
            return false;
        };

        for (int row = 0; row < n; ++row) {
            vector<bool> seen(n, false);
            augment(row, seen);
        }

        vector<pair<int, int>> matching(n);
        for (int col = 0; col < n; ++col) {
            int row = match_right[col];
            matching[row] = {row, col};
            in_matching[row][col] = true;
        }

        vector<pair<int, int>> extra;
        for (auto [row, col] : cells) {
            if (!in_matching[row][col]) extra.push_back({row, col});
        }

        int rem = m % n;
        vector<vector<pair<int, int>>> moves;

        if (rem == 0) {
            while (!extra.empty()) {
                vector<pair<int, int>> move;
                for (int i = 0; i < n; ++i) {
                    move.push_back(extra.back());
                    extra.pop_back();
                }
                moves.push_back(move);
            }
            moves.push_back(matching);
        } else {
            while (static_cast<int>(extra.size()) > rem) {
                vector<pair<int, int>> move;
                for (int i = 0; i < n; ++i) {
                    move.push_back(extra.back());
                    extra.pop_back();
                }
                moves.push_back(move);
            }

            vector<pair<int, int>> move = extra;
            for (int i = rem; i < n; ++i) move.push_back(matching[i]);
            moves.push_back(move);

            move.clear();
            for (int i = 0; i < rem; ++i) move.push_back(matching[i]);
            moves.push_back(move);
        }

        cout << moves.size() << '\n';
        for (const auto& move : moves) {
            cout << move.size() << '\n';
            for (auto [row, col] : move) {
                cout << row + 1 << ' ' << col + 1 << '\n';
            }
        }
    }
    return 0;
}
