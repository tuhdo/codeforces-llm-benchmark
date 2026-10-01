#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string mode;
    int t;
    cin >> mode >> t;

    while (t--) {
        int n;
        cin >> n;

        vector<string> picture(n);
        int black_count = 0;
        long long row_sum = 0;
        long long col_sum = 0;

        for (int r = 0; r < n; ++r) {
            cin >> picture[r];
            for (int c = 0; c < n; ++c) {
                if (picture[r][c] == '#') {
                    ++black_count;
                    row_sum += r;
                    col_sum += c;
                }
            }
        }

        if (mode == "second") {
            int inverse = 1;
            while (1LL * black_count * inverse % n != 1) {
                ++inverse;
            }

            cout << row_sum % n * inverse % n + 1 << ' '
                 << col_sum % n * inverse % n + 1 << '\n';
            continue;
        }

        int target_row, target_col;
        cin >> target_row >> target_col;
        --target_row;
        --target_col;

        int dr = (1LL * black_count * target_row - row_sum) % n;
        int dc = (1LL * black_count * target_col - col_sum) % n;
        if (dr < 0) dr += n;
        if (dc < 0) dc += n;

        if (dr == 0 && dc == 0) {
            cout << "1 1 1 1\n";
            continue;
        }

        bool found = false;
        for (int r = 0; r < n && !found; ++r) {
            for (int c = 0; c < n; ++c) {
                int next_row = (r + dr) % n;
                int next_col = (c + dc) % n;
                if (picture[r][c] == '#' &&
                    picture[next_row][next_col] == '.') {
                    cout << r + 1 << ' ' << c + 1 << ' '
                         << next_row + 1 << ' ' << next_col + 1 << '\n';
                    found = true;
                    break;
                }
            }
        }
    }

    return 0;
}
