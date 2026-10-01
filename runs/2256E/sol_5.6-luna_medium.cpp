#include <bits/stdc++.h>
using namespace std;

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
        vector<string> a(n);
        for (auto &row : a) cin >> row;

        if (mode == "first") {
            int rx, cx;
            cin >> rx >> cx;
            --rx;
            --cx;

            long long w = 0, sr = 0, sc = 0;
            for (int r = 0; r < n; ++r) {
                for (int c = 0; c < n; ++c) {
                    if (a[r][c] == '#') {
                        ++w;
                        sr = (sr + r) % n;
                        sc = (sc + c) % n;
                    }
                }
            }

            int dr = (int)((w * rx - sr) % n + n) % n;
            int dc = (int)((w * cx - sc) % n + n) % n;

            if (dr == 0 && dc == 0) {
                cout << 1 << ' ' << 1 << ' ' << 1 << ' ' << 1 << '\n';
                continue;
            }

            // Find a black cell p and a white cell p + (dr, dc).
            bool found = false;
            for (int r = 0; r < n && !found; ++r) {
                for (int c = 0; c < n && !found; ++c) {
                    if (a[r][c] != '#') continue;
                    int nr = (r + dr) % n;
                    int nc = (c + dc) % n;
                    if (a[nr][nc] == '.') {
                        cout << r + 1 << ' ' << c + 1 << ' '
                             << nr + 1 << ' ' << nc + 1 << '\n';
                        found = true;
                    }
                }
            }
            // Existence follows from gcd(n, w) = 1.
            if (!found) return 0;
        } else {
            long long w = 0, sr = 0, sc = 0;
            for (int r = 0; r < n; ++r) {
                for (int c = 0; c < n; ++c) {
                    if (a[r][c] == '#') {
                        ++w;
                        sr = (sr + r) % n;
                        sc = (sc + c) % n;
                    }
                }
            }

            long long inv = 1;
            while ((w * inv) % n != 1) ++inv;
            int rx = (int)(sr * inv % n);
            int cx = (int)(sc * inv % n);
            cout << rx + 1 << ' ' << cx + 1 << '\n';
        }
    }
    return 0;
}
