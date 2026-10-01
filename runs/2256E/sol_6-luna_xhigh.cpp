#include <bits/stdc++.h>
using namespace std;

long long extended_gcd(long long a, long long b, long long &x, long long &y) {
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

    string mode;
    if (!(cin >> mode)) return 0;

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<string> grid(n);
        for (string &row : grid) cin >> row;

        int black_count = 0;
        int sum_r = 0, sum_c = 0;
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (grid[r][c] == '#') {
                    ++black_count;
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

            int w_mod = black_count % n;
            int target_sum_r = static_cast<int>(1LL * w_mod * rx % n);
            int target_sum_c = static_cast<int>(1LL * w_mod * cx % n);
            int dr = (target_sum_r - sum_r + n) % n;
            int dc = (target_sum_c - sum_c + n) % n;

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
            int count_mod = black_count % n;
            int inv_count = inverse_mod(count_mod, n);
            int target_r = static_cast<int>(1LL * sum_r * inv_count % n);
            int target_c = static_cast<int>(1LL * sum_c * inv_count % n);
            cout << target_r + 1 << ' ' << target_c + 1 << '\n';
        }
    }
    return 0;
}
