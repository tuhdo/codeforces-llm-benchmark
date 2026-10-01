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

        long long w = 0, sr = 0, sc = 0;
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (a[r][c] == '#') {
                    ++w;
                    sr += r;
                    sc += c;
                }
            }
        }
        sr %= n;
        sc %= n;

        if (mode == "first") {
            int xr, xc;
            cin >> xr >> xc;
            --xr; --xc;

            int inv = 1;
            while ((w * inv) % n != 1) ++inv;
            int dr = (int)((1LL * w * xr - sr) % n);
            int dc = (int)((1LL * w * xc - sc) % n);
            if (dr < 0) dr += n;
            if (dc < 0) dc += n;

            if (dr == 0 && dc == 0) {
                cout << 1 << ' ' << 1 << ' ' << 1 << ' ' << 1 << '\n';
                continue;
            }

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
        } else {
            int ansr = (int)(sr * 1LL * [&]() {
                int z = 1;
                while ((w * z) % n != 1) ++z;
                return z;
            }() % n);
            int ansc = (int)(sc * 1LL * [&]() {
                int z = 1;
                while ((w * z) % n != 1) ++z;
                return z;
            }() % n);
            cout << ansr + 1 << ' ' << ansc + 1 << '\n';
        }
    }
}
