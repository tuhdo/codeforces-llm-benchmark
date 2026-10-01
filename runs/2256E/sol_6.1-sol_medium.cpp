#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string run;
    int t;
    cin >> run >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<string> grid(n);
        long long w = 0, row_sum = 0, col_sum = 0;
        for (int r = 0; r < n; ++r) {
            cin >> grid[r];
            for (int c = 0; c < n; ++c) {
                if (grid[r][c] == '#') {
                    ++w;
                    row_sum += r;
                    col_sum += c;
                }
            }
        }

        if (run == "first") {
            int target_r, target_c;
            cin >> target_r >> target_c;
            --target_r;
            --target_c;
            int dr = ((w * target_r - row_sum) % n + n) % n;
            int dc = ((w * target_c - col_sum) % n + n) % n;
            if (dr == 0 && dc == 0) {
                cout << "1 1 1 1\n";
                continue;
            }

            bool found = false;
            for (int r = 0; r < n && !found; ++r) {
                for (int c = 0; c < n; ++c) {
                    int nr = (r + dr) % n;
                    int nc = (c + dc) % n;
                    if (grid[r][c] == '#' && grid[nr][nc] == '.') {
                        cout << r + 1 << ' ' << c + 1 << ' '
                             << nr + 1 << ' ' << nc + 1 << '\n';
                        found = true;
                        break;
                    }
                }
            }
        } else {
            int inverse = 1;
            while (w * inverse % n != 1) {
                ++inverse;
            }
            cout << row_sum % n * inverse % n + 1 << ' '
                 << col_sum % n * inverse % n + 1 << '\n';
        }
    }
    return 0;
}
