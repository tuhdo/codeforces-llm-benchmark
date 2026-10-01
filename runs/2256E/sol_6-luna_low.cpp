#include <bits/stdc++.h>
using namespace std;

static long long mod_norm(long long x, int n) {
    x %= n;
    if (x < 0) x += n;
    return x;
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
        vector<string> a(n);
        for (auto &row : a) cin >> row;

        long long count = 0, sum_r = 0, sum_c = 0;
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (a[r][c] == '#') {
                    ++count;
                    sum_r += r;
                    sum_c += c;
                }
            }
        }

        if (mode == "first") {
            int rx, cx;
            cin >> rx >> cx;
            --rx;
            --cx;

            int dr = (int)mod_norm(count * rx - sum_r, n);
            int dc = (int)mod_norm(count * cx - sum_c, n);
            if (dr == 0 && dc == 0) {
                cout << "1 1 1 1\n";
                continue;
            }

            bool done = false;
            for (int r = 0; r < n && !done; ++r) {
                for (int c = 0; c < n && !done; ++c) {
                    if (a[r][c] != '#') continue;
                    int nr = (r + dr) % n;
                    int nc = (c + dc) % n;
                    if (a[nr][nc] == '.') {
                        cout << r + 1 << ' ' << c + 1 << ' '
                             << nr + 1 << ' ' << nc + 1 << '\n';
                        done = true;
                    }
                }
            }
            // Coprimality of n and the black-cell count guarantees such a pair.
        } else {
            long long q = count % n;
            long long inv = 1;
            while ((q * inv) % n != 1) ++inv;
            int xr = (int)(sum_r % n * inv % n);
            int xc = (int)(sum_c % n * inv % n);
            cout << xr + 1 << ' ' << xc + 1 << '\n';
        }
    }
    return 0;
}
