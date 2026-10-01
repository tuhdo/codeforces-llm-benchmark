#include <bits/stdc++.h>
using namespace std;

long long extended_gcd(long long a, long long b, long long& x, long long& y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    long long x1, y1;
    long long g = extended_gcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return g;
}

int inverse_mod(int a, int mod) {
    long long x, y;
    extended_gcd(a, mod, x, y);
    x %= mod;
    if (x < 0) x += mod;
    return static_cast<int>(x);
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

        long long black_count = 0;
        long long row_sum = 0, col_sum = 0;
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (picture[r][c] == '#') {
                    ++black_count;
                    row_sum += r;
                    col_sum += c;
                }
            }
        }

        int inverse = inverse_mod(static_cast<int>(black_count % n), n);

        if (run == "first") {
            int target_row, target_col;
            cin >> target_row >> target_col;
            --target_row;
            --target_col;

            int desired_row_sum = static_cast<int>((black_count % n) * target_row % n);
            int desired_col_sum = static_cast<int>((black_count % n) * target_col % n);
            int dr = (desired_row_sum - static_cast<int>(row_sum % n) + n) % n;
            int dc = (desired_col_sum - static_cast<int>(col_sum % n) + n) % n;

            if (dr == 0 && dc == 0) {
                cout << "1 1 1 1\n";
                continue;
            }

            bool found = false;
            for (int r = 0; r < n && !found; ++r) {
                for (int c = 0; c < n; ++c) {
                    if (picture[r][c] != '#') continue;
                    int nr = (r + dr) % n;
                    int nc = (c + dc) % n;
                    if (picture[nr][nc] == '.') {
                        cout << r + 1 << ' ' << c + 1 << ' ' << nr + 1 << ' ' << nc + 1 << '\n';
                        found = true;
                        break;
                    }
                }
            }
        } else {
            int target_row = static_cast<int>(row_sum % n) * inverse % n;
            int target_col = static_cast<int>(col_sum % n) * inverse % n;
            cout << target_row + 1 << ' ' << target_col + 1 << '\n';
        }
    }
}
