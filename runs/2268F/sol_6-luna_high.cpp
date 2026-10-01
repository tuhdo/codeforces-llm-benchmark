#include <bits/stdc++.h>
#include <cassert>
using namespace std;

using pii = pair<int, int>;

int n, m;
int a[202][202];
vector<pii> operations;

inline void operate(int i, int j) {
    swap(a[i][j], a[i + 1][j]);
    swap(a[i][j + 1], a[i + 1][j + 1]);
    operations.push_back({i, j});
}

inline int position_of(int x, int column) {
    for (int row = x; row <= m; ++row) {
        if (a[row][column] == x) return row;
    }
    assert(false);
    return -1;
}

// In an L/R pair, move the x in the right column to row x.
inline void move_right_x(int x, int j) {
    for (int row = position_of(x, j + 1) - 1; row >= x; --row)
        operate(row, j + 1);
}

// Align the x's in columns j+1 and j+2, then move them together to row x.
inline void align_right_left(int x, int j) {
    int p1 = position_of(x, j + 1);
    int p2 = position_of(x, j + 2);
    while (p1 > p2) operate(--p1, j);
    while (p1 < p2) operate(--p2, j + 2);
    while (p1 > x) operate(--p1, j + 1);
}

// In an R pair, move the x in the left column to row x.
inline void move_left_x(int x, int j) {
    for (int row = position_of(x, j) - 1; row >= x; --row)
        operate(row, j - 1);
}

// Handle an R pair whose left neighbor is already fixed.
inline void move_left_x_borrowing(int x, int j) {
    if (a[x + 1][j] == x) operate(x + 1, j);
    operate(x, j - 1);
    for (int row = position_of(x, j) - 1; row >= x + 1; --row)
        operate(row, j);
    operate(x, j - 1);
}

// Bring the two x's in a Type-0 pair to the same row, then lift together.
inline void fix_zero_pair(int x, int j) {
    int p1 = position_of(x, j);
    int p2 = position_of(x, j + 1);
    while (p1 > p2) operate(--p1, j - 1);
    while (p1 < p2) operate(--p2, j + 1);
    while (p1 > x) operate(--p1, j);
}

inline void fix_left_boundary(int x) {
    if (a[x][2] == x) {
        if (a[x][1] == x) return;
        if (a[x + 1][1] == x) operate(x + 1, 1);
        operate(x, 1);
    } else if (a[x][1] == x) {
        if (a[x + 1][2] == x) operate(x + 1, 1);
        operate(x, 1);
    }

    int p1 = position_of(x, 1), p2 = position_of(x, 2);
    if (p1 > p2) {
        while (p1 > p2) operate(--p1, 1);
        ++p2;
    }
    while (p1 < p2) operate(--p2, 2);
    while (p1 > x) operate(--p1, 1);
}

inline void fix_right_boundary(int x) {
    if (a[x][m - 1] == x) {
        if (a[x][m] == x) return;
        if (a[x + 1][m] == x) operate(x + 1, m - 1);
        operate(x, m - 1);
    } else if (a[x][m] == x) {
        if (a[x + 1][m - 1] == x) operate(x + 1, m - 1);
        operate(x, m - 1);
    }

    int p1 = position_of(x, m - 1), p2 = position_of(x, m);
    if (p1 < p2) {
        while (p1 < p2) operate(--p2, m - 1);
        ++p1;
    }
    while (p1 > p2) operate(--p1, m - 2);
    while (p1 > x) operate(--p1, m - 1);
}

inline void fix_last_row() {
    for (int j = 1; j < m; ++j) {
        if (a[m][j] != m) operate(m - 1, j);
    }
}

void solve_case() {
    cin >> n;
    m = 2 * n;
    operations.clear();

    for (int i = 1; i <= m; ++i)
        for (int j = 1; j <= m; ++j)
            cin >> a[i][j];

    int parity = 0;
    for (int j = 1; j <= m; ++j) {
        for (int r1 = 1; r1 <= m; ++r1) {
            for (int r2 = r1 + 1; r2 <= m; ++r2) {
                parity ^= (a[r1][j] > a[r2][j]);
            }
        }
    }

    if (parity) {
        cout << "-1\n";
        return;
    }

    if (n == 1) {
        if (a[1][1] == 2) cout << "1\n1 1\n";
        else cout << "0\n";
        return;
    }

    for (int x = 1; x <= m - 2; ++x) {
        // First clear pairs which already have both copies of x on top.
        for (int j = 3; j <= m - 3; j += 2) {
            if (a[x][j] == x && a[x][j + 1] == x)
                operate(x, j);
        }

        // Process L-pairs from left to right.
        for (int j = 3; j <= m - 3; j += 2) {
            if (a[x][j] == x && a[x][j + 1] != x) {
                if (j == m - 3 || a[x][j + 3] != x)
                    move_right_x(x, j);
                else
                    align_right_left(x, j);
            }
        }

        // Process R-pairs from right to left.
        for (int j = m - 3; j >= 3; j -= 2) {
            if (a[x][j] != x && a[x][j + 1] == x) {
                if (a[x][j - 2] == x && a[x][j - 1] == x)
                    move_left_x_borrowing(x, j);
                else
                    move_left_x(x, j);
            }
        }

        // Every remaining middle pair is Type 0 or Type 1.
        for (int j = 3; j <= m - 3; j += 2) {
            if (a[x][j] != x) fix_zero_pair(x, j);
        }

        fix_left_boundary(x);
        fix_right_boundary(x);
    }

    fix_last_row();

    cout << operations.size() << '\n';
    for (auto [i, j] : operations) cout << i << ' ' << j << '\n';
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) solve_case();
    return 0;
}
