#include <iostream>
#include <string>
#include <vector>

using namespace std;

int modulo(long long value, int n) {
    value %= n;
    if (value < 0) value += n;
    return static_cast<int>(value);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string phase;
    int t;
    cin >> phase >> t;

    while (t--) {
        int n;
        cin >> n;
        vector<string> picture(n);
        int black_count = 0;
        long long row_sum = 0, column_sum = 0;

        for (int r = 0; r < n; ++r) {
            cin >> picture[r];
            for (int c = 0; c < n; ++c) {
                if (picture[r][c] == '#') {
                    ++black_count;
                    row_sum += r;
                    column_sum += c;
                }
            }
        }

        if (phase == "first") {
            int target_row, target_column;
            cin >> target_row >> target_column;
            --target_row;
            --target_column;

            int dr = modulo(1LL * black_count * target_row - row_sum, n);
            int dc = modulo(1LL * black_count * target_column - column_sum, n);

            if (dr == 0 && dc == 0) {
                cout << "1 1 1 1\n";
                continue;
            }

            // Invariance under this nonzero shift would make its cycle
            // length divide both n and black_count, contradicting their gcd.
            bool found = false;
            for (int r = 0; r < n && !found; ++r) {
                for (int c = 0; c < n; ++c) {
                    int next_row = (r + dr) % n;
                    int next_column = (c + dc) % n;
                    if (picture[r][c] == '#' &&
                        picture[next_row][next_column] == '.') {
                        cout << r + 1 << ' ' << c + 1 << ' '
                             << next_row + 1 << ' ' << next_column + 1 << '\n';
                        found = true;
                        break;
                    }
                }
            }
        } else {
            int inverse = 1;
            while ((black_count % n) * inverse % n != 1) ++inverse;

            // The modular centroid also survives color inversion: both
            // the count and coordinate sums change sign modulo n.
            cout << modulo(row_sum * inverse, n) + 1 << ' '
                 << modulo(column_sum * inverse, n) + 1 << '\n';
        }
    }
}
