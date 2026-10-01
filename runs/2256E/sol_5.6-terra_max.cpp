#include <bits/stdc++.h>

using namespace std;

struct Point {
    int r;
    int c;
};

int modInverse(int a, int mod) {
    int b = mod;
    int x = 1;
    int y = 0;

    while (b != 0) {
        const int q = a / b;
        const int nextA = a - q * b;
        a = b;
        b = nextA;

        const int nextX = x - q * y;
        x = y;
        y = nextX;
    }

    x %= mod;
    if (x < 0) {
        x += mod;
    }
    return x;
}

Point centroid(const vector<string>& picture) {
    const int n = static_cast<int>(picture.size());
    int blackCount = 0;
    int rowSum = 0;
    int colSum = 0;

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (picture[r][c] == '#') {
                ++blackCount;
                rowSum = (rowSum + r) % n;
                colSum = (colSum + c) % n;
            }
        }
    }

    const int inverse = modInverse(blackCount % n, n);
    return {(rowSum * inverse) % n, (colSum * inverse) % n};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string run;
    cin >> run;

    int testCount;
    cin >> testCount;

    while (testCount--) {
        int n;
        cin >> n;

        vector<string> picture(n);
        for (string& row : picture) {
            cin >> row;
        }

        if (run == "second") {
            const Point answer = centroid(picture);
            cout << answer.r + 1 << ' ' << answer.c + 1 << '\n';
            continue;
        }

        int targetRow;
        int targetCol;
        cin >> targetRow >> targetCol;
        --targetRow;
        --targetCol;

        int blackCount = 0;
        for (const string& row : picture) {
            blackCount += static_cast<int>(count(row.begin(), row.end(), '#'));
        }

        const Point center = centroid(picture);
        const int rowMove = (blackCount * ((targetRow - center.r + n) % n)) % n;
        const int colMove = (blackCount * ((targetCol - center.c + n) % n)) % n;

        if (rowMove == 0 && colMove == 0) {
            cout << "1 1 1 1\n";
            continue;
        }

        bool found = false;
        for (int r = 0; r < n && !found; ++r) {
            for (int c = 0; c < n; ++c) {
                const int nr = (r + rowMove) % n;
                const int nc = (c + colMove) % n;
                if (picture[r][c] == '#' && picture[nr][nc] == '.') {
                    cout << r + 1 << ' ' << c + 1 << ' ' << nr + 1 << ' ' << nc + 1 << '\n';
                    found = true;
                    break;
                }
            }
        }
    }

    return 0;
}
