#include <bits/stdc++.h>
using namespace std;

static long long mod_inverse(long long a, long long mod) {
    long long old_r = a, r = mod;
    long long old_s = 1, s = 0;
    while (r != 0) {
        long long q = old_r / r;
        long long next_r = old_r - q * r;
        old_r = r;
        r = next_r;
        long long next_s = old_s - q * s;
        old_s = s;
        s = next_s;
    }
    old_s %= mod;
    if (old_s < 0) old_s += mod;
    return old_s;
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
        for (string &row : grid) cin >> row;

        if (mode == "first") {
            int rx, cx;
            cin >> rx >> cx;
            --rx;
            --cx;

            long long black_count = 0;
            long long sum_r = 0, sum_c = 0;
            for (int r = 0; r < n; ++r) {
                for (int c = 0; c < n; ++c) {
                    if (grid[r][c] == '#') {
                        ++black_count;
                        sum_r = (sum_r + r) % n;
                        sum_c = (sum_c + c) % n;
                    }
                }
            }

            int dr = (black_count * rx - sum_r) % n;
            int dc = (black_count * cx - sum_c) % n;
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
                    if (grid[r][c] == '#' && grid[nr][nc] == '.') {
                        cout << r + 1 << ' ' << c + 1 << ' '
                             << nr + 1 << ' ' << nc + 1 << '\n';
                        found = true;
                        break;
                    }
                }
            }
        } else {
            long long black_count = 0;
            long long sum_r = 0, sum_c = 0;
            for (int r = 0; r < n; ++r) {
                for (int c = 0; c < n; ++c) {
                    if (grid[r][c] == '#') {
                        ++black_count;
                        sum_r = (sum_r + r) % n;
                        sum_c = (sum_c + c) % n;
                    }
                }
            }

            long long inv_count = mod_inverse(black_count % n, n);
            int rx = sum_r * inv_count % n;
            int cx = sum_c * inv_count % n;
            cout << rx + 1 << ' ' << cx + 1 << '\n';
        }
    }
    return 0;
}
