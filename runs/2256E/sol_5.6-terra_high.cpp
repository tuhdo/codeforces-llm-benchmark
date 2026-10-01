#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int modInverse(int a, int mod) {
    int b = mod, u = 1, v = 0;
    while (b != 0) {
        int t = a / b;
        a -= t * b;
        swap(a, b);
        u -= t * v;
        swap(u, v);
    }
    u %= mod;
    if (u < 0) u += mod;
    return u;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string run;
    cin >> run;
    int t;
    cin >> t;

    while (t--) {
        int n;
        cin >> n;
        vector<string> picture(n);
        for (string& row : picture) cin >> row;

        int black = 0;
        int sumRow = 0, sumCol = 0;
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (picture[r][c] == '#') {
                    ++black;
                    sumRow += r;
                    sumCol += c;
                }
            }
        }
        sumRow %= n;
        sumCol %= n;

        if (run == "second") {
            int inverse = modInverse(black % n, n);
            int answerRow = static_cast<int64>(sumRow) * inverse % n;
            int answerCol = static_cast<int64>(sumCol) * inverse % n;
            cout << answerRow + 1 << ' ' << answerCol + 1 << '\n';
            continue;
        }

        int targetRow, targetCol;
        cin >> targetRow >> targetCol;
        --targetRow;
        --targetCol;

        int deltaRow = (static_cast<int64>(black) * targetRow - sumRow) % n;
        int deltaCol = (static_cast<int64>(black) * targetCol - sumCol) % n;
        if (deltaRow < 0) deltaRow += n;
        if (deltaCol < 0) deltaCol += n;

        if (deltaRow == 0 && deltaCol == 0) {
            cout << "1 1 1 1\n";
            continue;
        }

        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                int nr = (r + deltaRow) % n;
                int nc = (c + deltaCol) % n;
                if (picture[r][c] == '#' && picture[nr][nc] == '.') {
                    cout << r + 1 << ' ' << c + 1 << ' ' << nr + 1 << ' ' << nc + 1 << '\n';
                    goto found_swap;
                }
            }
        }
        // The required pair always exists: otherwise this non-zero translation
        // would preserve the black set, whose orbit size divides both n and black.
        return 1;
    found_swap:
        ;
    }
    return 0;
}
