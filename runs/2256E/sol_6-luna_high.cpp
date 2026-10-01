#include <bits/stdc++.h>
using namespace std;

static int mod_norm(long long x, int mod) {
    x %= mod;
    if (x < 0) x += mod;
    return static_cast<int>(x);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string mode;
    if (!(cin >> mode)) return 0;
    int t;
    cin >> t;

    while (t--) {
        int n;
        cin >> n;
        vector<string> grid(n);
        for (auto &row : grid) cin >> row;

        if (mode == "first") {
            int rx, cx;
            cin >> rx >> cx;
            --rx;
            --cx;

            long long w = 0, sr = 0, sc = 0;
            for (int r = 0; r < n; ++r) {
                for (int c = 0; c < n; ++c) {
                    if (grid[r][c] == '#') {
                        ++w;
                        sr += r;
                        sc += c;
                    }
                }
            }

            int dr = mod_norm(w * rx - sr, n);
            int dc = mod_norm(w * cx - sc, n);

            if (dr == 0 && dc == 0) {
                cout << rx + 1 << ' ' << cx + 1 << ' '
                     << rx + 1 << ' ' << cx + 1 << '\n';
                continue;
            }

            bool found = false;
            for (int r = 0; r < n && !found; ++r) {
                for (int c = 0; c < n && !found; ++c) {
                    if (grid[r][c] != '#') continue;
                    int nr = (r + dr) % n;
                    int nc = (c + dc) % n;
                    if (grid[nr][nc] == '.') {
                        cout << r + 1 << ' ' << c + 1 << ' '
                             << nr + 1 << ' ' << nc + 1 << '\n';
                        found = true;
                    }
                }
            }
        } else {
            long long w = 0, sr = 0, sc = 0;
            for (int r = 0; r < n; ++r) {
                for (int c = 0; c < n; ++c) {
                    if (grid[r][c] == '#') {
                        ++w;
                        sr += r;
                        sc += c;
                    }
                }
            }

            int wm = mod_norm(w, n);
            int inv = 1;
            while ((1LL * inv * wm) % n != 1) ++inv;
            int rx = static_cast<int>(1LL * mod_norm(sr, n) * inv % n);
            int cx = static_cast<int>(1LL * mod_norm(sc, n) * inv % n);
            cout << rx + 1 << ' ' << cx + 1 << '\n';
        }
    }
    return 0;
}
