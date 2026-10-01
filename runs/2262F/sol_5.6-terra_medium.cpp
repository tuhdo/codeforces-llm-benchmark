#include <bits/stdc++.h>
#include <cassert>
using namespace std;

struct Move {
    vector<pair<int, int>> cells;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n, m;
        cin >> n >> m;
        vector<pair<int, int>> edges(m);
        vector<vector<pair<int, int>>> graph(n);
        for (int i = 0; i < m; ++i) {
            int x, y;
            cin >> x >> y;
            --x; --y;
            edges[i] = {x, y};
            graph[x].push_back({y, i});
        }

        // A full-rank square matrix has a perfect matching in its support.
        vector<int> matchCol(n, -1), matchEdge(n, -1);
        function<bool(int, vector<char>&)> augment = [&](int row, vector<char>& seen) {
            for (auto [col, id] : graph[row]) {
                if (seen[col]) continue;
                seen[col] = true;
                if (matchCol[col] == -1 || augment(matchCol[col], seen)) {
                    matchCol[col] = row;
                    matchEdge[col] = id;
                    return true;
                }
            }
            return false;
        };
        for (int row = 0; row < n; ++row) {
            vector<char> seen(n);
            bool ok = augment(row, seen);
            assert(ok);
        }

        vector<char> inMatching(m);
        vector<pair<int, int>> matching;
        for (int col = 0; col < n; ++col) {
            inMatching[matchEdge[col]] = true;
            matching.push_back(edges[matchEdge[col]]);
        }

        vector<pair<int, int>> extra;
        for (int id = 0; id < m; ++id)
            if (!inMatching[id]) extra.push_back(edges[id]);

        vector<Move> answer;
        size_t pos = 0;
        while (extra.size() - pos >= static_cast<size_t>(n)) {
            Move move;
            move.cells.insert(move.cells.end(), extra.begin() + pos,
                              extra.begin() + pos + n);
            pos += n;
            answer.push_back(move);
        }

        int leftExtra = static_cast<int>(extra.size() - pos);
        if (leftExtra > 0) {
            Move move;
            move.cells.insert(move.cells.end(), extra.begin() + pos, extra.end());
            // Keep leftExtra matching cells, and remove the other n-leftExtra.
            move.cells.insert(move.cells.end(), matching.begin() + leftExtra, matching.end());
            answer.push_back(move);

            Move last;
            last.cells.insert(last.cells.end(), matching.begin(), matching.begin() + leftExtra);
            answer.push_back(last);
        } else {
            Move last;
            last.cells = matching;
            answer.push_back(last);
        }

        cout << answer.size() << '\n';
        for (const Move& move : answer) {
            cout << move.cells.size();
            for (auto [x, y] : move.cells)
                cout << ' ' << x + 1 << ' ' << y + 1;
            cout << '\n';
        }
    }
}
