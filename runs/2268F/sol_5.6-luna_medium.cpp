#include <bits/stdc++.h>
#include <cassert>
using namespace std;

using pii = pair<int, int>;

static int n, m;
static int a[205][205];
static vector<pii> answer;

static inline void op(int i, int j) {
    swap(a[i][j], a[i + 1][j]);
    swap(a[i][j + 1], a[i + 1][j + 1]);
    answer.push_back({i, j});
}

static int where(int x, int col) {
    for (int row = x; row <= m; ++row) {
        if (a[row][col] == x) return row;
    }
    assert(false);
    return -1;
}

// Move x in column col+1 to row x, using the pair (col, col+1).
static void moveRight(int x, int col) {
    for (int row = where(x, col + 1) - 1; row >= x; --row) op(row, col + 1);
}

// Equalize the depths of x in columns col+1 and col+2, then lift both.
static void joinRight(int x, int col) {
    int p = where(x, col + 1), q = where(x, col + 2);
    while (p > q) op(--p, col);
    while (p < q) op(--q, col + 2);
    while (p > x) op(--p, col + 1);
}

// Move x in the left column of pair (col-1,col) to row x.
static void moveLeft(int x, int col) {
    for (int row = where(x, col) - 1; row >= x; --row) op(row, col - 1);
}

// Handle a left singleton when its left neighboring pair is already fixed.
static void moveLeftBorrowing(int x, int col) {
    if (a[x + 1][col] == x) op(x + 1, col);
    op(x, col - 1);
    for (int row = where(x, col) - 1; row >= x + 1; --row) op(row, col);
    op(x, col - 1);
}

// Resolve a zero pair (col,col+1): equalize the two depths and lift them.
static void resolveZero(int x, int col) {
    int p = where(x, col), q = where(x, col + 1);
    while (p > q) op(--p, col - 1);
    while (p < q) op(--q, col + 1);
    while (p > x) op(--p, col);
}

static void fixFirstPair(int x) {
    if (a[x][2] == x) {
        if (a[x][1] == x) return;
        if (a[x + 1][1] == x) op(x + 1, 1);
        op(x, 1);
    } else if (a[x][1] == x) {
        if (a[x + 1][2] == x) op(x + 1, 1);
        op(x, 1);
    }

    int p = where(x, 1), q = where(x, 2);
    if (p > q) {
        while (p > q) op(--p, 1);
        ++q; // column 2 was moved down once by the last operation
    }
    while (p < q) op(--q, 2);
    while (p > x) op(--p, 1);
}

static void fixLastPair(int x) {
    if (a[x][m - 1] == x) {
        if (a[x][m] == x) return;
        if (a[x + 1][m] == x) op(x + 1, m - 1);
        op(x, m - 1);
    } else if (a[x][m] == x) {
        if (a[x + 1][m - 1] == x) op(x + 1, m - 1);
        op(x, m - 1);
    }

    int p = where(x, m - 1), q = where(x, m);
    if (p < q) {
        while (p < q) op(--q, m - 1);
        ++p;
    }
    while (p > q) op(--p, m - 2);
    while (p > x) op(--p, m - 1);
}

static void finishLastRow() {
    for (int col = 1; col < m; ++col) {
        if (a[m][col] != m) op(m - 1, col);
    }
}

static bool solveCase() {
    for (int i = 1; i <= m; ++i)
        for (int j = 1; j <= m; ++j)
            cin >> a[i][j];

    int parity = 0;
    for (int col = 1; col <= m; ++col)
        for (int i = 1; i <= m; ++i)
            for (int j = i + 1; j <= m; ++j)
                parity ^= (a[i][col] > a[j][col]);
    if (parity) return false;

    if (n == 1) {
        if (a[1][1] == 2) answer.push_back({1, 1});
        return true;
    }

    for (int x = 1; x <= m - 2; ++x) {
        // First remove type-1 middle pairs, so they can be used as type-0.
        for (int col = 3; col <= m - 3; col += 2)
            if (a[x][col] == x && a[x][col + 1] == x) op(x, col);

        // Process left singletons from left to right.
        for (int col = 3; col <= m - 3; col += 2) {
            if (a[x][col] == x && a[x][col + 1] != x) {
                if (col == m - 3 || a[x][col + 3] != x)
                    moveRight(x, col);
                else
                    joinRight(x, col);
            }
        }

        // Process right singletons from right to left.
        for (int col = m - 3; col >= 3; col -= 2) {
            if (a[x][col] != x && a[x][col + 1] == x) {
                if (a[x][col - 2] == x && a[x][col - 1] == x)
                    moveLeftBorrowing(x, col);
                else
                    moveLeft(x, col);
            }
        }

        // Every remaining middle pair is type 0 or type 1.
        for (int col = 3; col <= m - 3; col += 2)
            if (a[x][col] != x) resolveZero(x, col);

        fixFirstPair(x);
        fixLastPair(x);
    }
    finishLastRow();
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        cin >> n;
        m = 2 * n;
        answer.clear();

        if (!solveCase()) {
            cout << -1 << '\n';
            continue;
        }

        cout << answer.size() << '\n';
        for (auto [i, j] : answer) cout << i << ' ' << j << '\n';
    }
    return 0;
}
