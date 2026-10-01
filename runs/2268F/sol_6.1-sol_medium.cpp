#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>
using namespace std;

class Solver {
    int m;
    vector<vector<int>> a, pos;
    vector<pair<int, int>> moves;

    void apply(int row, int col) {
        assert(1 <= row && row < m && 1 <= col && col < m);
        for (int c = col; c <= col + 1; ++c) {
            swap(a[row][c], a[row + 1][c]);
            pos[c][a[row][c]] = row;
            pos[c][a[row + 1][c]] = row + 1;
        }
        moves.emplace_back(row, col);
    }

    void lift(int value, int col, int edge, int target) {
        while (pos[col][value] > target)
            apply(pos[col][value] - 1, edge);
    }

    // Both tokens are below the current row. Equalize their depths using
    // the outside edges, then lift them together on the inside edge.
    void align(int value, int left, int leftEdge, int rightEdge) {
        int depth = min(pos[left][value], pos[left + 1][value]);
        lift(value, left, leftEdge, depth);
        lift(value, left + 1, rightEdge, depth);
        lift(value, left, left, value);
    }

    // The two border pairs have only one outside edge. Reflect the same
    // construction for the right border to avoid duplicating the cases.
    void border(int value, bool right) {
        int outer = right ? m : 1;
        int inner = right ? m - 1 : 2;
        int pairEdge = right ? m - 1 : 1;
        int outsideEdge = right ? m - 2 : 2;
        bool outerDone = pos[outer][value] == value;
        bool innerDone = pos[inner][value] == value;
        if (outerDone && innerDone) return;

        if (outerDone != innerDone) {
            int missing = outerDone ? inner : outer;
            if (pos[missing][value] == value + 1)
                apply(value + 1, pairEdge);
            apply(value, pairEdge);
        }

        // A shared swap may move the inner token down. The position table
        // tracks that change, including the last swap that crosses it.
        while (pos[outer][value] > pos[inner][value])
            apply(pos[outer][value] - 1, pairEdge);
        lift(value, inner, outsideEdge, pos[outer][value]);
        lift(value, outer, pairEdge, value);
    }

    void fixRow(int value) {
        // Temporarily turn completed middle pairs into pairs with no
        // token in this row, so they can serve as unfinished neighbors.
        for (int c = 3; c <= m - 3; c += 2)
            if (pos[c][value] == value && pos[c + 1][value] == value)
                apply(value, c);

        // Pairs with only their left token placed are handled left to right.
        for (int c = 3; c <= m - 3; c += 2) {
            if (pos[c][value] != value || pos[c + 1][value] == value)
                continue;
            if (c == m - 3 || pos[c + 3][value] != value) {
                lift(value, c + 1, c + 1, value);
            } else {
                // An adjacent left-only/right-only pair can be completed
                // by lifting its two missing inner tokens together.
                align(value, c + 1, c, c + 2);
            }
        }

        // Pairs with only their right token placed are handled right to left.
        for (int c = m - 3; c >= 3; c -= 2) {
            if (pos[c][value] == value || pos[c + 1][value] != value)
                continue;
            bool previousDone = pos[c - 2][value] == value
                             && pos[c - 1][value] == value;
            if (!previousDone) {
                lift(value, c, c - 1, value);
            } else {
                // Borrow the preceding placed token, then restore it.
                if (pos[c][value] == value + 1) apply(value + 1, c);
                apply(value, c - 1);
                lift(value, c, c, value + 1);
                apply(value, c - 1);
            }
        }

        for (int c = 3; c <= m - 3; c += 2)
            if (pos[c][value] != value)
                align(value, c, c - 1, c + 1);

        border(value, false);
        border(value, true);
    }

public:
    explicit Solver(int n) : m(2 * n), a(m + 1, vector<int>(m + 1)),
                            pos(m + 1, vector<int>(m + 1)) {
        for (int r = 1; r <= m; ++r)
            for (int c = 1; c <= m; ++c) {
                cin >> a[r][c];
                pos[c][a[r][c]] = r;
            }
    }

    void solve() {
        int parity = 0;
        for (int c = 1; c <= m; ++c)
            for (int r = 1; r <= m; ++r)
                for (int s = r + 1; s <= m; ++s)
                    parity ^= (a[r][c] > a[s][c]);
        if (parity) {
            cout << -1 << '\n';
            return;
        }

        if (m > 2)
            for (int value = 1; value <= m - 2; ++value)
                fixRow(value);

        // Only the final two values remain. Push each reversed column's
        // defect rightward; even total parity guarantees the last is fixed.
        for (int c = 1; c < m; ++c)
            if (a[m][c] != m) apply(m - 1, c);

        assert(moves.size() <= size_t((m / 2) * (m * (m - 1) / 2) + 9 * (m / 2)));
        for (int r = 1; r <= m; ++r)
            for (int c = 1; c <= m; ++c)
                assert(a[r][c] == r);
        cout << moves.size() << '\n';
        for (auto [r, c] : moves) cout << r << ' ' << c << '\n';
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        Solver(n).solve();
    }
}
