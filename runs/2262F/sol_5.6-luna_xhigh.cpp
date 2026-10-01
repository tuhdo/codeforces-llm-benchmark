#include <bits/stdc++.h>
using namespace std;

constexpr int MAX_N = 305;
using Cell = pair<int, int>;

static vector<Cell> find_perfect_matching(
    const vector<bitset<MAX_N>> &active,
    int n
) {
    vector<int> column_match(n, -1);

    auto augment = [&](auto &&self, int row, vector<char> &seen) -> bool {
        for (int col = 0; col < n; ++col) {
            if (!active[row][col] || seen[col]) {
                continue;
            }
            seen[col] = true;
            if (column_match[col] == -1 ||
                self(self, column_match[col], seen)) {
                column_match[col] = row;
                return true;
            }
        }
        return false;
    };

    for (int row = 0; row < n; ++row) {
        vector<char> seen(n, false);
        augment(augment, row, seen);
    }

    vector<Cell> matching;
    matching.reserve(n);
    for (int col = 0; col < n; ++col) {
        matching.emplace_back(column_match[col], col);
    }
    return matching;
}

static void remove_one(
    vector<bitset<MAX_N>> &active,
    vector<bitset<MAX_N>> &inverse_rows,
    vector<bitset<MAX_N>> &inverse_cols,
    int row,
    int col
) {
    const int n = (int)active.size();
    const bitset<MAX_N> column = inverse_cols[row];
    const bitset<MAX_N> inverse_row = inverse_rows[col];

    for (int i = 0; i < n; ++i) {
        if (column[i]) {
            inverse_rows[i] ^= inverse_row;
        }
    }
    for (int j = 0; j < n; ++j) {
        if (inverse_row[j]) {
            inverse_cols[j] ^= column;
        }
    }
    active[row].reset(col);
}

static void remove_two_same_row(
    vector<bitset<MAX_N>> &active,
    vector<bitset<MAX_N>> &inverse_rows,
    vector<bitset<MAX_N>> &inverse_cols,
    int row,
    int col1,
    int col2
) {
    const int n = (int)active.size();
    const bitset<MAX_N> column = inverse_cols[row];
    const bitset<MAX_N> inverse_row = inverse_rows[col1] ^ inverse_rows[col2];

    for (int i = 0; i < n; ++i) {
        if (column[i]) {
            inverse_rows[i] ^= inverse_row;
        }
    }
    for (int j = 0; j < n; ++j) {
        if (inverse_row[j]) {
            inverse_cols[j] ^= column;
        }
    }
    active[row].reset(col1);
    active[row].reset(col2);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int test_cases;
    cin >> test_cases;

    while (test_cases--) {
        int n, m;
        cin >> n >> m;

        vector<bitset<MAX_N>> active(n);
        for (int i = 0; i < m; ++i) {
            int row, col;
            cin >> row >> col;
            active[--row][--col] = 1;
        }

        vector<bitset<2 * MAX_N>> augmented(n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                augmented[i][j] = active[i][j];
            }
            augmented[i][n + i] = 1;
        }
        for (int col = 0; col < n; ++col) {
            int pivot = col;
            while (pivot < n && !augmented[pivot][col]) {
                ++pivot;
            }
            swap(augmented[col], augmented[pivot]);
            for (int row = 0; row < n; ++row) {
                if (row != col && augmented[row][col]) {
                    augmented[row] ^= augmented[col];
                }
            }
        }

        vector<bitset<MAX_N>> inverse_rows(n);
        vector<bitset<MAX_N>> inverse_cols(n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                inverse_rows[i][j] = augmented[i][n + j];
                if (inverse_rows[i][j]) {
                    inverse_cols[j][i] = 1;
                }
            }
        }

        vector<vector<Cell>> moves;
        int ones = m;

        while (ones >= 2 * n) {
            vector<Cell> move;
            move.reserve(n);
            int need = n;

            while (need > 0) {
                Cell single = {-1, -1};
                array<int, 3> same_row_pair = {-1, -1, -1};

                for (int row = 0; row < n; ++row) {
                    bitset<MAX_N> noncritical =
                        active[row] & ~inverse_cols[row];
                    if (single.first == -1 && noncritical.any()) {
                        for (int col = 0; col < n; ++col) {
                            if (noncritical[col]) {
                                single = {row, col};
                                break;
                            }
                        }
                    }

                    bitset<MAX_N> critical =
                        active[row] & inverse_cols[row];
                    if (same_row_pair[0] == -1 && critical.count() >= 2) {
                        int first = -1;
                        for (int col = 0; col < n; ++col) {
                            if (!critical[col]) {
                                continue;
                            }
                            if (first == -1) {
                                first = col;
                            } else {
                                same_row_pair = {row, first, col};
                                break;
                            }
                        }
                    }
                }

                if ((need & 1) && single.first != -1) {
                    move.push_back(single);
                    remove_one(
                        active,
                        inverse_rows,
                        inverse_cols,
                        single.first,
                        single.second
                    );
                    --need;
                    --ones;
                } else if (need >= 2 && same_row_pair[0] != -1) {
                    int row = same_row_pair[0];
                    int col1 = same_row_pair[1];
                    int col2 = same_row_pair[2];
                    move.emplace_back(row, col1);
                    move.emplace_back(row, col2);
                    remove_two_same_row(
                        active,
                        inverse_rows,
                        inverse_cols,
                        row,
                        col1,
                        col2
                    );
                    need -= 2;
                    ones -= 2;
                } else if (single.first != -1) {
                    move.push_back(single);
                    remove_one(
                        active,
                        inverse_rows,
                        inverse_cols,
                        single.first,
                        single.second
                    );
                    --need;
                    --ones;
                } else {
                    // This cannot occur while ones >= 2*n and need > 0.
                    return 0;
                }
            }
            moves.push_back(move);
        }

        vector<Cell> remaining;
        remaining.reserve(ones);
        for (int row = 0; row < n; ++row) {
            for (int col = 0; col < n; ++col) {
                if (active[row][col]) {
                    remaining.emplace_back(row, col);
                }
            }
        }

        vector<Cell> matching = find_perfect_matching(active, n);
        int remainder = ones % n;
        if (remainder == 0) {
            moves.push_back(remaining);
        } else {
            vector<char> keep(n * n, false);
            for (int i = 0; i < remainder; ++i) {
                keep[matching[i].first * n + matching[i].second] = true;
            }

            vector<Cell> rank_n_move;
            vector<Cell> last_move;
            rank_n_move.reserve(n);
            last_move.reserve(remainder);
            for (Cell cell : remaining) {
                if (keep[cell.first * n + cell.second]) {
                    last_move.push_back(cell);
                } else {
                    rank_n_move.push_back(cell);
                }
            }
            moves.push_back(rank_n_move);
            moves.push_back(last_move);
        }

        cout << moves.size() << '\n';
        for (const auto &move : moves) {
            cout << move.size() << '\n';
            for (auto [row, col] : move) {
                cout << row + 1 << ' ' << col + 1 << '\n';
            }
        }
    }
    return 0;
}
