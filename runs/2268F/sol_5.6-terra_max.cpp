#include <bits/stdc++.h>
#include <cassert>
using namespace std;

using Move = pair<int, int>;

int n, m;
vector<vector<int>> a;
vector<Move> answer;

void apply_move(int row, int col) {
    swap(a[row][col], a[row + 1][col]);
    swap(a[row][col + 1], a[row + 1][col + 1]);
    answer.push_back({row, col});
}

int position_of(int value, int col) {
    for (int row = value; row <= m; ++row) {
        if (a[row][col] == value) return row;
    }
    assert(false);
    return -1;
}

// The left cell of pair (col, col + 1) already contains x.
// Bring the x in col + 1 to row x, using the pair to its right.
void fix_left_only(int x, int col) {
    for (int row = position_of(x, col + 1) - 1; row >= x; --row) {
        apply_move(row, col + 1);
    }
}

// An L-pair at (col, col + 1) is followed by an R-pair.
void fix_left_right(int x, int col) {
    int p = position_of(x, col + 1);
    int q = position_of(x, col + 2);

    while (p > q) apply_move(--p, col);
    while (p < q) apply_move(--q, col + 2);
    while (p > x) apply_move(--p, col + 1);
}

// The right cell of pair (col, col + 1) already contains x.
// Bring the x in col to row x, using the pair to its left.
void fix_right_only(int x, int col) {
    for (int row = position_of(x, col) - 1; row >= x; --row) {
        apply_move(row, col - 1);
    }
}

// The pair to the left is already fixed, so temporarily borrow its right column.
void fix_right_after_fixed_pair(int x, int col) {
    if (a[x + 1][col] == x) apply_move(x + 1, col);
    apply_move(x, col - 1);
    for (int row = position_of(x, col) - 1; row >= x + 1; --row) {
        apply_move(row, col);
    }
    apply_move(x, col - 1);
}

// Both x's of a middle pair are below row x. Align them, then lift them together.
void fix_empty_middle_pair(int x, int col) {
    int p = position_of(x, col);
    int q = position_of(x, col + 1);

    while (p > q) apply_move(--p, col - 1);
    while (p < q) apply_move(--q, col + 1);
    while (p > x) apply_move(--p, col);
}

void fix_first_pair(int x) {
    if (a[x][2] == x) {
        if (a[x][1] == x) return;
        if (a[x + 1][1] == x) apply_move(x + 1, 1);
        apply_move(x, 1);
    } else if (a[x][1] == x) {
        if (a[x + 1][2] == x) apply_move(x + 1, 1);
        apply_move(x, 1);
    }

    int p = position_of(x, 1);
    int q = position_of(x, 2);

    if (p > q) {
        while (p > q) apply_move(--p, 1);
        ++q;
    }
    while (p < q) apply_move(--q, 2);
    while (p > x) apply_move(--p, 1);
}

void fix_last_pair(int x) {
    if (a[x][m - 1] == x) {
        if (a[x][m] == x) return;
        if (a[x + 1][m] == x) apply_move(x + 1, m - 1);
        apply_move(x, m - 1);
    } else if (a[x][m] == x) {
        if (a[x + 1][m - 1] == x) apply_move(x + 1, m - 1);
        apply_move(x, m - 1);
    }

    int p = position_of(x, m - 1);
    int q = position_of(x, m);

    if (p < q) {
        while (p < q) apply_move(--q, m - 1);
        ++p;
    }
    while (p > q) apply_move(--p, m - 2);
    while (p > x) apply_move(--p, m - 1);
}

bool has_odd_total_inversion_parity() {
    int parity = 0;
    for (int col = 1; col <= m; ++col) {
        for (int upper = 1; upper <= m; ++upper) {
            for (int lower = upper + 1; lower <= m; ++lower) {
                parity ^= (a[upper][col] > a[lower][col]);
            }
        }
    }
    return parity;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int test_count;
    cin >> test_count;

    while (test_count--) {
        cin >> n;
        m = 2 * n;
        a.assign(m + 1, vector<int>(m + 1));
        for (int row = 1; row <= m; ++row) {
            for (int col = 1; col <= m; ++col) {
                cin >> a[row][col];
            }
        }

        answer.clear();
        if (has_odd_total_inversion_parity()) {
            cout << "-1\n";
            continue;
        }

        if (n == 1) {
            if (a[1][1] == 2) {
                cout << "1\n1 1\n";
            } else {
                cout << "0\n";
            }
            continue;
        }

        for (int x = 1; x <= m - 2; ++x) {
            // Work on all pairs except the two border pairs.
            for (int col = 3; col <= m - 3; col += 2) {
                if (a[x][col] == x && a[x][col + 1] == x) {
                    apply_move(x, col);
                }
            }

            // Resolve L-pairs from left to right.
            for (int col = 3; col <= m - 3; col += 2) {
                if (a[x][col] == x && a[x][col + 1] != x) {
                    if (col == m - 3 || a[x][col + 3] != x) {
                        fix_left_only(x, col);
                    } else {
                        fix_left_right(x, col);
                    }
                }
            }

            // Resolve R-pairs from right to left.
            for (int col = m - 3; col >= 3; col -= 2) {
                if (a[x][col] != x && a[x][col + 1] == x) {
                    if (a[x][col - 2] == x && a[x][col - 1] == x) {
                        fix_right_after_fixed_pair(x, col);
                    } else {
                        fix_right_only(x, col);
                    }
                }
            }

            for (int col = 3; col <= m - 3; col += 2) {
                if (a[x][col] != x) {
                    fix_empty_middle_pair(x, col);
                }
            }

            fix_first_pair(x);
            fix_last_pair(x);
        }

        // Only m - 1 and m remain in each column. Slide every bad column right.
        for (int col = 1; col < m; ++col) {
            if (a[m][col] != m) {
                apply_move(m - 1, col);
            }
        }

        cout << answer.size() << '\n';
        for (auto [row, col] : answer) {
            cout << row << ' ' << col << '\n';
        }
    }
}
