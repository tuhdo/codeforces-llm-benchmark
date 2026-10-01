#include <bits/stdc++.h>
#include <cassert>
using namespace std;

using Move = pair<int, int>;

int n, m;
int a[202][202];
vector<Move> answer;

void apply(int row, int col) {
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

// Complete an L pair using the pair immediately to its right.
void fix_left(int value, int col) {
    for (int row = position_of(value, col + 1) - 1; row >= value; --row) {
        apply(row, col + 1);
    }
}

// Complete neighboring L and R pairs simultaneously.
void fix_left_right(int value, int col) {
    int p1 = position_of(value, col + 1);
    int p2 = position_of(value, col + 2);
    while (p1 > p2) apply(--p1, col);
    while (p1 < p2) apply(--p2, col + 2);
    while (p1 > value) apply(--p1, col + 1);
}

// Complete an R pair using the pair immediately to its left.
void fix_right(int value, int col) {
    for (int row = position_of(value, col) - 1; row >= value; --row) {
        apply(row, col - 1);
    }
}

void fix_right_after_full_left(int value, int col) {
    if (a[value + 1][col] == value) apply(value + 1, col);
    apply(value, col - 1);
    for (int row = position_of(value, col) - 1; row >= value + 1; --row) {
        apply(row, col);
    }
    apply(value, col - 1);
}

// The pair has no copy of value in its current top row.
void fix_empty_pair(int value, int col) {
    int p1 = position_of(value, col);
    int p2 = position_of(value, col + 1);
    while (p1 > p2) apply(--p1, col - 1);
    while (p1 < p2) apply(--p2, col + 1);
    while (p1 > value) apply(--p1, col);
}

void fix_first_pair(int value) {
    if (a[value][2] == value) {
        if (a[value][1] == value) return;
        if (a[value + 1][1] == value) apply(value + 1, 1);
        apply(value, 1);
    } else if (a[value][1] == value) {
        if (a[value + 1][2] == value) apply(value + 1, 1);
        apply(value, 1);
    }

    int p1 = position_of(value, 1);
    int p2 = position_of(value, 2);
    if (p1 > p2) {
        while (p1 > p2) apply(--p1, 1);
        ++p2;
    }
    while (p1 < p2) apply(--p2, 2);
    while (p1 > value) apply(--p1, 1);
}

void fix_last_pair(int value) {
    if (a[value][m - 1] == value) {
        if (a[value][m] == value) return;
        if (a[value + 1][m] == value) apply(value + 1, m - 1);
        apply(value, m - 1);
    } else if (a[value][m] == value) {
        if (a[value + 1][m - 1] == value) apply(value + 1, m - 1);
        apply(value, m - 1);
    }

    int p1 = position_of(value, m - 1);
    int p2 = position_of(value, m);
    if (p1 < p2) {
        while (p1 < p2) apply(--p2, m - 1);
        ++p1;
    }
    while (p1 > p2) apply(--p1, m - 2);
    while (p1 > value) apply(--p1, m - 1);
}

void fix_penultimate_row() {
    for (int col = 1; col < m; ++col) {
        if (a[m - 1][col] != m - 1) apply(m - 1, col);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        cin >> n;
        m = 2 * n;
        for (int row = 1; row <= m; ++row) {
            for (int col = 1; col <= m; ++col) cin >> a[row][col];
        }

        int parity = 0;
        for (int col = 1; col <= m; ++col) {
            for (int r1 = 1; r1 <= m; ++r1) {
                for (int r2 = r1 + 1; r2 <= m; ++r2) {
                    parity ^= (a[r1][col] > a[r2][col]);
                }
            }
        }
        if (parity) {
            cout << -1 << '\n';
            continue;
        }

        answer.clear();
        if (n == 1) {
            if (a[1][1] == 2) cout << "1\n1 1\n";
            else cout << "0\n";
            continue;
        }

        for (int value = 1; value <= m - 2; ++value) {
            // First remove full middle pairs, turning them into empty pairs.
            for (int col = 3; col <= m - 3; col += 2) {
                if (a[value][col] == value && a[value][col + 1] == value) {
                    apply(value, col);
                }
            }

            // Sweep L pairs left to right.
            for (int col = 3; col <= m - 3; col += 2) {
                if (a[value][col] == value && a[value][col + 1] != value) {
                    if (col == m - 3 || a[value][col + 3] != value) {
                        fix_left(value, col);
                    } else {
                        fix_left_right(value, col);
                    }
                }
            }

            // Sweep R pairs right to left.
            for (int col = m - 3; col >= 3; col -= 2) {
                if (a[value][col] != value && a[value][col + 1] == value) {
                    if (a[value][col - 2] == value && a[value][col - 1] == value) {
                        fix_right_after_full_left(value, col);
                    } else {
                        fix_right(value, col);
                    }
                }
            }

            for (int col = 3; col <= m - 3; col += 2) {
                if (a[value][col] != value) fix_empty_pair(value, col);
            }

            fix_first_pair(value);
            fix_last_pair(value);
        }

        fix_penultimate_row();
        cout << answer.size() << '\n';
        for (auto [row, col] : answer) cout << row << ' ' << col << '\n';
    }
    return 0;
}
