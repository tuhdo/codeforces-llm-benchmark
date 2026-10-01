#include <bits/stdc++.h>
using namespace std;

int inverse_mod(int a, int mod) {
    int b = mod, x = 1, y = 0;
    while (b != 0) {
        int q = a / b;
        tie(a, b) = make_pair(b, a - q * b);
        tie(x, y) = make_pair(y, x - q * y);
    }
    x %= mod;
    if (x < 0) x += mod;
    return x;
}

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
        for (string &row : a) cin >> row;

        int black = 0;
        for (const string &row : a)
            black += count(row.begin(), row.end(), '#');

        // The rarer color is preserved even if the jury inverts every color.
        const char marked = (2 * black < n * n ? '#' : '.');
        const int k = (marked == '#' ? black : n * n - black);

        int sum_r = 0, sum_c = 0;
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (a[r][c] == marked) {
                    sum_r = (sum_r + r) % n;
                    sum_c = (sum_c + c) % n;
                }
            }
        }

        if (mode == "first") {
            int rx, cx;
            cin >> rx >> cx;
            --rx;
            --cx;

            int dr = (rx * k - sum_r) % n;
            int dc = (cx * k - sum_c) % n;
            if (dr < 0) dr += n;
            if (dc < 0) dc += n;

            for (int r = 0; r < n; ++r) {
                for (int c = 0; c < n; ++c) {
                    int nr = (r + dr) % n;
                    int nc = (c + dc) % n;
                    if (a[r][c] == marked && a[nr][nc] != marked) {
                        cout << r + 1 << ' ' << c + 1 << ' '
                             << nr + 1 << ' ' << nc + 1 << '\n';
                        goto done_case;
                    }
                }
            }
            // dr = dc = 0 reaches this case. Swapping a cell with itself is valid.
            for (int r = 0; r < n; ++r) {
                for (int c = 0; c < n; ++c) {
                    if (a[r][c] == marked) {
                        cout << r + 1 << ' ' << c + 1 << ' '
                             << r + 1 << ' ' << c + 1 << '\n';
                        goto done_case;
                    }
                }
            }
        } else {
            int inv = inverse_mod(k, n);
            cout << (sum_r * inv) % n + 1 << ' '
                 << (sum_c * inv) % n + 1 << '\n';
        }

    done_case:;
    }
    return 0;
}
