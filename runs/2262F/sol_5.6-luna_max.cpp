#include <bits/stdc++.h>
#include <cassert>
using namespace std;

constexpr int MAX_N = 305;
using Bits = bitset<MAX_N>;

struct Cell {
    int row;
    int col;
};

class MatrixState {
public:
    explicit MatrixState(int n) : n_(n), matrix_(n), inverse_(n), ones_(0) {}

    int n() const {
        return n_;
    }

    int ones() const {
        return ones_;
    }

    void set_one(int row, int col) {
        if (!matrix_[row][col]) {
            matrix_[row].set(col);
            ++ones_;
        }
    }

    bool has_one(int row, int col) const {
        return matrix_[row][col];
    }

    const Bits& row_bits(int row) const {
        return matrix_[row];
    }

    void rebuild_inverse() {
        vector<Bits> work = matrix_;
        inverse_.assign(n_, Bits());
        for (int i = 0; i < n_; ++i) {
            inverse_[i].set(i);
        }

        for (int col = 0; col < n_; ++col) {
            int pivot = col;
            while (pivot < n_ && !work[pivot][col]) {
                ++pivot;
            }
            assert(pivot < n_);
            swap(work[col], work[pivot]);
            swap(inverse_[col], inverse_[pivot]);

            for (int row = 0; row < n_; ++row) {
                if (row != col && work[row][col]) {
                    work[row] ^= work[col];
                    inverse_[row] ^= inverse_[col];
                }
            }
        }
    }

    bool inverse_entry(int row, int col) const {
        return inverse_[row][col];
    }

    const Bits& inverse_row(int row) const {
        return inverse_[row];
    }

    // Delete entries from one row. The inverse update is valid when the
    // selected columns have an even dot product with inverse_[*][row].
    void delete_from_row(int row, const vector<int>& cols) {
        Bits update;
        for (int col : cols) {
            update ^= inverse_[col];
        }

        for (int inverse_row = 0; inverse_row < n_; ++inverse_row) {
            if (inverse_[inverse_row][row]) {
                inverse_[inverse_row] ^= update;
            }
        }

        for (int col : cols) {
            assert(matrix_[row][col]);
            matrix_[row].reset(col);
            --ones_;
        }
    }

    void delete_cell(int row, int col) {
        assert(matrix_[row][col]);
        matrix_[row].reset(col);
        --ones_;
    }

    void delete_pair(Cell first, Cell second) {
        const int r1 = first.row;
        const int c1 = first.col;
        const int r2 = second.row;
        const int c2 = second.col;

        const int k00 = 1 ^ static_cast<int>(inverse_[c1][r1]);
        const int k01 = static_cast<int>(inverse_[c1][r2]);
        const int k10 = static_cast<int>(inverse_[c2][r1]);
        const int k11 = 1 ^ static_cast<int>(inverse_[c2][r2]);
        assert(((k00 & k11) ^ (k01 & k10)) == 1);

        const Bits q0 = inverse_[c1];
        const Bits q1 = inverse_[c2];
        for (int row = 0; row < n_; ++row) {
            Bits update;
            if (inverse_[row][r1]) {
                if (k11) {
                    update ^= q0;
                }
                if (k01) {
                    update ^= q1;
                }
            }
            if (inverse_[row][r2]) {
                if (k10) {
                    update ^= q0;
                }
                if (k00) {
                    update ^= q1;
                }
            }
            inverse_[row] ^= update;
        }

        assert(matrix_[r1][c1]);
        assert(matrix_[r2][c2]);
        matrix_[r1].reset(c1);
        matrix_[r2].reset(c2);
        ones_ -= 2;
    }

private:
    int n_;
    vector<Bits> matrix_;
    vector<Bits> inverse_;
    int ones_;
};

static optional<Cell> find_single_removal(const MatrixState& state) {
    for (int row = 0; row < state.n(); ++row) {
        for (int col = 0; col < state.n(); ++col) {
            if (state.has_one(row, col) && !state.inverse_entry(col, row)) {
                return Cell{row, col};
            }
        }
    }
    return nullopt;
}

static bool pair_preserves_rank(const MatrixState& state, Cell first, Cell second) {
    const int r1 = first.row;
    const int c1 = first.col;
    const int r2 = second.row;
    const int c2 = second.col;

    if (r1 == r2) {
        return state.inverse_entry(c1, r1) == state.inverse_entry(c2, r2);
    }
    if (c1 == c2) {
        return state.inverse_entry(c1, r1) == state.inverse_entry(c2, r2);
    }

    const int k00 = 1 ^ static_cast<int>(state.inverse_entry(c1, r1));
    const int k01 = static_cast<int>(state.inverse_entry(c1, r2));
    const int k10 = static_cast<int>(state.inverse_entry(c2, r1));
    const int k11 = 1 ^ static_cast<int>(state.inverse_entry(c2, r2));
    return ((k00 & k11) ^ (k01 & k10)) == 1;
}

static optional<pair<Cell, Cell>> find_good_pair(const MatrixState& state) {
    for (int row = 0; row < state.n(); ++row) {
        int previous[2] = {-1, -1};
        for (int col = 0; col < state.n(); ++col) {
            if (!state.has_one(row, col)) {
                continue;
            }
            int value = static_cast<int>(state.inverse_entry(col, row));
            if (previous[value] != -1) {
                return pair<Cell, Cell>(Cell{row, previous[value]}, Cell{row, col});
            }
            previous[value] = col;
        }
    }

    vector<Cell> cells;
    cells.reserve(state.ones());
    for (int row = 0; row < state.n(); ++row) {
        for (int col = 0; col < state.n(); ++col) {
            if (state.has_one(row, col)) {
                cells.push_back(Cell{row, col});
            }
        }
    }

    // If no same-row pair was found, every row has at most two entries, so
    // this search has only O(n^2) candidates.
    for (int i = 0; i < static_cast<int>(cells.size()); ++i) {
        for (int j = i + 1; j < static_cast<int>(cells.size()); ++j) {
            if (pair_preserves_rank(state, cells[i], cells[j])) {
                return pair<Cell, Cell>(cells[i], cells[j]);
            }
        }
    }
    return nullopt;
}

// Called when every current 1-entry is critical by itself. It finds a
// same-row pair whose deletion makes a single removable entry appear.
static optional<pair<Cell, Cell>> find_pair_creating_single(const MatrixState& state) {
    const int n = state.n();
    vector<Bits> inverse_columns(n);
    for (int col = 0; col < n; ++col) {
        for (int row = 0; row < n; ++row) {
            if (state.inverse_entry(row, col)) {
                inverse_columns[col].set(row);
            }
        }
    }

    vector<Bits> useful_rows(n);
    for (int col = 0; col < n; ++col) {
        for (int row = 0; row < n; ++row) {
            if ((state.row_bits(row) & inverse_columns[col]).any()) {
                useful_rows[col].set(row);
            }
        }
    }

    for (int row = 0; row < n; ++row) {
        vector<int> cols;
        for (int col = 0; col < n; ++col) {
            if (state.has_one(row, col)) {
                cols.push_back(col);
            }
        }
        if (cols.size() < 2) {
            continue;
        }

        Bits first_mask = state.inverse_row(cols[0]) & useful_rows[row];
        for (int index = 1; index < static_cast<int>(cols.size()); ++index) {
            Bits current_mask = state.inverse_row(cols[index]) & useful_rows[row];
            if (current_mask != first_mask) {
                return pair<Cell, Cell>(Cell{row, cols[0]}, Cell{row, cols[index]});
            }
        }
    }
    return nullopt;
}

static vector<Cell> remove_exactly_n_while_full_rank(MatrixState& state) {
    const int n = state.n();
    int need = n;
    vector<Cell> removed;
    removed.reserve(n);

    while (need > 3) {
        optional<Cell> single = find_single_removal(state);
        if (single.has_value()) {
            removed.push_back(*single);
            state.delete_from_row(single->row, vector<int>{single->col});
            --need;
            continue;
        }

        int row = -1;
        vector<int> cols;
        for (int candidate = 0; candidate < n; ++candidate) {
            cols.clear();
            for (int col = 0; col < n; ++col) {
                if (state.has_one(candidate, col)) {
                    cols.push_back(col);
                }
            }
            if (cols.size() >= 2) {
                row = candidate;
                break;
            }
        }
        assert(row != -1);
        removed.push_back(Cell{row, cols[0]});
        removed.push_back(Cell{row, cols[1]});
        state.delete_from_row(row, vector<int>{cols[0], cols[1]});
        need -= 2;
    }

    if (need == 2) {
        optional<pair<Cell, Cell>> pair = find_good_pair(state);
        assert(pair.has_value());
        removed.push_back(pair->first);
        removed.push_back(pair->second);
        state.delete_pair(pair->first, pair->second);
    } else if (need == 3) {
        optional<Cell> single = find_single_removal(state);
        if (single.has_value()) {
            removed.push_back(*single);
            state.delete_from_row(single->row, vector<int>{single->col});

            optional<pair<Cell, Cell>> pair = find_good_pair(state);
            assert(pair.has_value());
            removed.push_back(pair->first);
            removed.push_back(pair->second);
            state.delete_pair(pair->first, pair->second);
        } else {
            optional<pair<Cell, Cell>> pair = find_pair_creating_single(state);
            assert(pair.has_value());
            removed.push_back(pair->first);
            removed.push_back(pair->second);
            state.delete_from_row(pair->first.row,
                                  vector<int>{pair->first.col, pair->second.col});

            single = find_single_removal(state);
            assert(single.has_value());
            removed.push_back(*single);
            state.delete_from_row(single->row, vector<int>{single->col});
        }
    }

    assert(static_cast<int>(removed.size()) == n);
    return removed;
}

static vector<Cell> find_perfect_matching(const MatrixState& state) {
    const int n = state.n();
    vector<int> matched_row_for_col(n, -1);
    vector<char> used(n);

    function<bool(int)> augment = [&](int row) {
        if (used[row]) {
            return false;
        }
        used[row] = true;
        for (int col = 0; col < n; ++col) {
            if (!state.has_one(row, col)) {
                continue;
            }
            if (matched_row_for_col[col] == -1 || augment(matched_row_for_col[col])) {
                matched_row_for_col[col] = row;
                return true;
            }
        }
        return false;
    };

    for (int row = 0; row < n; ++row) {
        fill(used.begin(), used.end(), false);
        assert(augment(row));
    }

    vector<Cell> matching(n);
    for (int col = 0; col < n; ++col) {
        matching[matched_row_for_col[col]] = Cell{matched_row_for_col[col], col};
    }
    return matching;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n, m;
        cin >> n >> m;

        MatrixState state(n);

        for (int i = 0; i < m; ++i) {
            int x, y;
            cin >> x >> y;
            --x;
            --y;
            state.set_one(x, y);
        }
        state.rebuild_inverse();

        int quotient = m / n;
        int remainder = m % n;
        vector<vector<Cell>> moves;

        if (remainder == 0) {
            for (int move_index = 0; move_index + 1 < quotient; ++move_index) {
                moves.push_back(remove_exactly_n_while_full_rank(state));
            }

            vector<Cell> last;
            for (int row = 0; row < n; ++row) {
                for (int col = 0; col < n; ++col) {
                    if (state.has_one(row, col)) {
                        last.push_back(Cell{row, col});
                    }
                }
            }
            assert(static_cast<int>(last.size()) == n);
            moves.push_back(last);
        } else {
            for (int move_index = 0; move_index + 1 < quotient; ++move_index) {
                moves.push_back(remove_exactly_n_while_full_rank(state));
            }

            vector<Cell> matching = find_perfect_matching(state);
            vector<vector<char>> keep(n, vector<char>(n, false));
            for (int i = 0; i < remainder; ++i) {
                keep[matching[i].row][matching[i].col] = true;
            }

            vector<Cell> penultimate;
            vector<Cell> last;
            for (int row = 0; row < n; ++row) {
                for (int col = 0; col < n; ++col) {
                    if (!state.has_one(row, col)) {
                        continue;
                    }
                    Cell cell{row, col};
                    if (keep[row][col]) {
                        last.push_back(cell);
                    } else {
                        penultimate.push_back(cell);
                        state.delete_cell(row, col);
                    }
                }
            }
            assert(static_cast<int>(penultimate.size()) == n);
            assert(static_cast<int>(last.size()) == remainder);
            moves.push_back(penultimate);
            moves.push_back(last);
        }

        cout << moves.size() << '\n';
        for (const auto& move : moves) {
            cout << move.size() << '\n';
            for (const Cell& cell : move) {
                cout << cell.row + 1 << ' ' << cell.col + 1 << '\n';
            }
        }
    }

    return 0;
}
