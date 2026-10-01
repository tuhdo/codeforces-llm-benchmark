#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int inverse_mod(int a, int modulus) {
    int b = modulus, x = 1, y = 0;
    while (b != 0) {
        int q = a / b;
        int next_a = a - q * b;
        a = b;
        b = next_a;
        int next_x = x - q * y;
        x = y;
        y = next_x;
    }
    return (x % modulus + modulus) % modulus;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string mode;
    int t;
    cin >> mode >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<string> grid(n);
        int black = 0;
        long long row_sum = 0, column_sum = 0;
        for (int r = 0; r < n; ++r) {
            cin >> grid[r];
            for (int c = 0; c < n; ++c) {
                if (grid[r][c] == '#') {
                    ++black;
                    row_sum += r;
                    column_sum += c;
                }
            }
        }

        if (mode == "second") {
            int inv = inverse_mod(black % n, n);
            cout << row_sum % n * inv % n + 1 << ' '
                 << column_sum % n * inv % n + 1 << '\n';
            continue;
        }

        int target_r, target_c;
        cin >> target_r >> target_c;
        --target_r;
        --target_c;
        int dr = (black % n * target_r - row_sum % n + n) % n;
        int dc = (black % n * target_c - column_sum % n + n) % n;
        if (dr == 0 && dc == 0) {
            cout << "1 1 1 1\n";
            continue;
        }

        // Move a black cell by the displacement needed to correct both sums.
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
        // Otherwise the black set would be translation-invariant, which
        // contradicts gcd(n, black) = 1 for a nonzero displacement.
        assert(found);
    }
}
