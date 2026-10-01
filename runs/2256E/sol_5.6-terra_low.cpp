#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string mode;
    if (!(cin >> mode)) return 0;
    int tests;
    cin >> tests;

    while (tests--) {
        int n;
        cin >> n;
        vector<string> picture(n);
        for (string &row : picture) cin >> row;

        long long black = 0;
        int sumRow = 0, sumCol = 0;
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (picture[r][c] == '#') {
                    ++black;
                    sumRow = (sumRow + r) % n;
                    sumCol = (sumCol + c) % n;
                }
            }
        }

        if (mode == "first") {
            int xr, xc;
            cin >> xr >> xc;
            --xr;
            --xc;

            int dr = (static_cast<long long>(black % n) * xr - sumRow) % n;
            int dc = (static_cast<long long>(black % n) * xc - sumCol) % n;
            if (dr < 0) dr += n;
            if (dc < 0) dc += n;

            if (dr == 0 && dc == 0) {
                cout << "1 1 1 1\n";
                continue;
            }

            // There is a black p for which p + (dr, dc) is white.
            // Otherwise the black set is invariant under this nonzero shift;
            // every orbit length divides n and would divide black as well.
            bool done = false;
            for (int r = 0; r < n && !done; ++r) {
                for (int c = 0; c < n; ++c) {
                    int nr = (r + dr) % n;
                    int nc = (c + dc) % n;
                    if (picture[r][c] == '#' && picture[nr][nc] == '.') {
                        cout << r + 1 << ' ' << c + 1 << ' ' << nr + 1 << ' ' << nc + 1 << '\n';
                        done = true;
                        break;
                    }
                }
            }
        } else {
            // n is not necessarily prime, so use extended Euclid.
            long long a = black % n, b = n, u = 1, v = 0;
            while (b) {
                long long q = a / b;
                a -= q * b;
                swap(a, b);
                u -= q * v;
                swap(u, v);
            }
            long long inverseBlack = (u % n + n) % n;
            cout << static_cast<long long>(sumRow) * inverseBlack % n + 1 << ' '
                 << static_cast<long long>(sumCol) * inverseBlack % n + 1 << '\n';
        }
    }
}
