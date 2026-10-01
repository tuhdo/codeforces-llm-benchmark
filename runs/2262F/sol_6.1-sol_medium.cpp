#include <bits/stdc++.h>
using namespace std;

constexpr int MAX_N = 300;
using Cell = pair<int, int>;

struct Solver {
    int n, remaining;
    vector<vector<int>> rows;
    vector<bitset<MAX_N>> present;
    vector<bitset<MAX_N>> inverse;
    vector<vector<Cell>> moves;

    void build_inverse() {
        vector<bitset<2 * MAX_N>> a(n);
        for (int i = 0; i < n; ++i) {
            for (int j : rows[i]) a[i].set(j);
            a[i].set(n + i);
        }
        for (int j = 0; j < n; ++j) {
            int pivot = j;
            while (!a[pivot][j]) ++pivot;
            swap(a[j], a[pivot]);
            for (int i = 0; i < n; ++i)
                if (i != j && a[i][j]) a[i] ^= a[j];
        }
        inverse.resize(n);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                inverse[i][j] = a[i][n + j];
    }

    Cell find_free() const {
        for (int i = 0; i < n; ++i)
            for (int j : rows[i])
                if (!inverse[j][i]) return {i, j};
        return {-1, -1};
    }

    void erase_cell(int i, int j, vector<Cell>& move) {
        auto it = find(rows[i].begin(), rows[i].end(), j);
        *it = rows[i].back();
        rows[i].pop_back();
        present[i].reset(j);
        --remaining;
        move.emplace_back(i, j);
    }

    // All updates here have v^T B e_i = 0, so the inverse remains valid.
    void update_inverse(int i, const bitset<MAX_N>& delta) {
        for (int j = 0; j < n; ++j)
            if (inverse[j][i]) inverse[j] ^= delta;
    }

    void erase_one(int i, int j, vector<Cell>& move) {
        bitset<MAX_N> delta = inverse[j];
        update_inverse(i, delta);
        erase_cell(i, j, move);
    }

    void erase_pair(int i, int j, int k, vector<Cell>& move) {
        bitset<MAX_N> delta = inverse[j] ^ inverse[k];
        update_inverse(i, delta);
        erase_cell(i, j, move);
        erase_cell(i, k, move);
    }

    void remove_two(vector<Cell>& move) {
        for (int i = 0; i < n; ++i) {
            if (rows[i].size() < 3) continue;
            int a = rows[i][0], b = rows[i][1], c = rows[i][2];
            if (inverse[a][i] == inverse[b][i]) erase_pair(i, a, b, move);
            else if (inverse[a][i] == inverse[c][i]) erase_pair(i, a, c, move);
            else erase_pair(i, b, c, move);
            return;
        }
        // With at most two ones per row, an invertible matrix has a unique
        // perfect matching. Every other one can be removed independently.
        for (int rep = 0; rep < 2; ++rep) {
            auto [i, j] = find_free();
            erase_one(i, j, move);
        }
    }

    void remove_odd(vector<Cell>& move) {
        auto [i, j] = find_free();
        if (i != -1) {
            erase_one(i, j, move);
            return;
        }

        // Every present entry has cofactor one. Find a pair in one row
        // whose removal makes a third entry free.
        i = 0;
        while (rows[i].size() < 3) ++i;
        int p = -1;
        for (int candidate : rows[i]) {
            for (int r = 0; r < n; ++r) {
                if (r != i && present[r][candidate]) {
                    j = candidate;
                    p = r;
                    break;
                }
            }
            if (p != -1) break;
        }
        int k = -1;
        for (int candidate : rows[i]) {
            if (!inverse[candidate][p]) {
                k = candidate;
                break;
            }
        }
        erase_pair(i, j, k, move);
        erase_one(p, j, move);
    }

    void finish() {
        if (remaining == n) {
            vector<Cell> move;
            for (int i = 0; i < n; ++i)
                for (int j : rows[i]) move.emplace_back(i, j);
            moves.push_back(move);
            return;
        }

        vector<int> matched_row(n, -1), seen(n);
        function<bool(int)> augment = [&](int i) {
            for (int j : rows[i]) {
                if (seen[j]) continue;
                seen[j] = 1;
                if (matched_row[j] == -1 || augment(matched_row[j])) {
                    matched_row[j] = i;
                    return true;
                }
            }
            return false;
        };
        for (int i = 0; i < n; ++i) {
            fill(seen.begin(), seen.end(), 0);
            augment(i);
        }
        int q = remaining - n;
        vector<int> keep(n, -1);
        vector<Cell> last;
        for (int j = 0; j < n && int(last.size()) < q; ++j) {
            int i = matched_row[j];
            keep[i] = j;
            last.emplace_back(i, j);
        }
        vector<Cell> move;
        for (int i = 0; i < n; ++i)
            for (int j : rows[i])
                if (keep[i] != j) move.emplace_back(i, j);
        moves.push_back(move);
        moves.push_back(last);
    }

    void solve() {
        cin >> n >> remaining;
        rows.resize(n);
        present.resize(n);
        for (int e = 0; e < remaining; ++e) {
            int i, j;
            cin >> i >> j;
            rows[i - 1].push_back(j - 1);
            present[i - 1].set(j - 1);
        }
        build_inverse();
        while (remaining >= 2 * n) {
            vector<Cell> move;
            if (n % 2) remove_odd(move);
            while (int(move.size()) < n) remove_two(move);
            moves.push_back(move);
        }
        finish();
        cout << moves.size() << '\n';
        for (const auto& move : moves) {
            cout << move.size();
            for (auto [i, j] : move) cout << ' ' << i + 1 << ' ' << j + 1;
            cout << '\n';
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    cin >> t;
    while (t--) Solver().solve();
}
