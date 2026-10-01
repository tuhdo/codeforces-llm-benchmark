#include <bits/stdc++.h>
using namespace std;

struct Move { int r, c; };

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        const int m = 2 * n;
        vector<vector<int>> a(m, vector<int>(m));
        for (auto &row : a) for (int &x : row) cin >> x;

        auto inv = [&]() {
            int z = 0;
            for (int c = 0; c < m; ++c)
                for (int i = 0; i < m; ++i)
                    for (int j = i + 1; j < m; ++j)
                        z += a[i][c] > a[j][c];
            return z;
        };
        int initial = inv();

        // With two rows and two columns every move affects both columns in
        // exactly the same way.
        if (n == 1) {
            if (a[0][0] == a[0][1] && a[1][0] == a[1][1]) {
                if (a[0][0] == 1) cout << 0 << '\n';
                else cout << 1 << "\n1 1\n";
            } else {
                cout << -1 << '\n';
            }
            continue;
        }
        if (initial & 1) {
            cout << -1 << '\n';
            continue;
        }

        vector<Move> ans;
        const int extraLimit = 9 * n;
        int neutral = 0;

        auto descent = [&](int r, int c) { return a[r][c] > a[r + 1][c]; };
        auto apply = [&](int r, int c) {
            swap(a[r][c], a[r + 1][c]);
            swap(a[r][c + 1], a[r + 1][c + 1]);
            ans.push_back({r, c});
        };
        auto goodCount = [&]() {
            int z = 0;
            for (int r = 0; r + 1 < m; ++r)
                for (int c = 0; c + 1 < m; ++c)
                    z += descent(r, c) && descent(r, c + 1);
            return z;
        };
        auto hasDescent = [&]() {
            for (int r = 0; r + 1 < m; ++r)
                for (int c = 0; c < m; ++c)
                    if (descent(r, c)) return true;
            return false;
        };

        bool ok = true;
        while (true) {
            bool made = false;
            for (int r = 0; r + 1 < m && !made; ++r) {
                for (int c = 0; c + 1 < m; ++c) {
                    if (descent(r, c) && descent(r, c + 1)) {
                        apply(r, c);
                        made = true;
                        break;
                    }
                }
            }
            if (made) continue;

            if (!hasDescent()) break;
            if (neutral == extraLimit) { ok = false; break; }

            // A neutral move moves one adjacent inversion to a neighbouring
            // column.  Prefer a move that immediately creates a reducible pair.
            int br = -1, bc = -1, best = -1;
            for (int r = 0; r + 1 < m; ++r) for (int c = 0; c + 1 < m; ++c) {
                if (descent(r, c) == descent(r, c + 1)) continue;
                swap(a[r][c], a[r + 1][c]);
                swap(a[r][c + 1], a[r + 1][c + 1]);
                int score = goodCount();
                swap(a[r][c], a[r + 1][c]);
                swap(a[r][c + 1], a[r + 1][c + 1]);
                if (score > best) best = score, br = r, bc = c;
            }
            if (br < 0) { ok = false; break; }
            apply(br, bc);
            ++neutral;
        }

        const long long limit = 1LL * n * m * (m - 1) / 2 + 9LL * n;
        if (!ok || (long long)ans.size() > limit) {
            cout << -1 << '\n';
        } else {
            cout << ans.size() << '\n';
            for (auto [r, c] : ans) cout << r + 1 << ' ' << c + 1 << '\n';
        }
    }
}
