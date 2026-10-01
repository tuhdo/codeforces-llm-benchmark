#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string mode;
    cin >> mode;
    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<string> a(n);
        long long w = 0, sr = 0, sc = 0;
        for (int r = 0; r < n; ++r) {
            cin >> a[r];
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
            int x, y;
            cin >> x >> y;
            --x;
            --y;
            int dr = (w * x - sr) % n;
            int dc = (w * y - sc) % n;
            if (dr < 0) dr += n;
            if (dc < 0) dc += n;
            if (dr == 0 && dc == 0) {
                cout << "1 1 1 1\n";
                continue;
            }
            bool found = false;
            for (int r = 0; r < n && !found; ++r) {
                for (int c = 0; c < n; ++c) {
                    int nr = (r + dr) % n;
                    int nc = (c + dc) % n;
                    if (a[r][c] == '#' && a[nr][nc] == '.') {
                        cout << r + 1 << ' ' << c + 1 << ' '
                             << nr + 1 << ' ' << nc + 1 << '\n';
                        found = true;
                        break;
                    }
                }
            }
        } else {
            int inv = 1;
            while ((w % n) * inv % n != 1) ++inv;
            cout << sr * inv % n + 1 << ' ' << sc * inv % n + 1 << '\n';
        }
    }
}
