#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<int> b(n + 1), difference(n + 2, 0);
        for (int i = 1; i <= n; ++i) cin >> b[i];

        // A position closer than b[i] to a known positive distance cannot
        // contain treasure, otherwise that known distance would be too small.
        for (int i = 1; i <= n; ++i) {
            if (b[i] <= 0) continue;
            int left = max(1, i - b[i] + 1);
            int right = min(n, i + b[i] - 1);
            if (left <= right) {
                ++difference[left];
                --difference[right + 1];
            }
        }

        vector<bool> allowed(n + 1), treasure(n + 1, false);
        int covered = 0;
        for (int i = 1; i <= n; ++i) {
            covered += difference[i];
            allowed[i] = (covered == 0);
        }

        bool possible = true;
        for (int i = 1; i <= n; ++i) {
            if (b[i] == 0) {
                if (!allowed[i]) possible = false;
                treasure[i] = true;
            }
        }

        for (int i = 1; i <= n && possible; ++i) {
            if (b[i] <= 0) continue;
            int left = i - b[i], right = i + b[i];
            if (left >= 1 && allowed[left]) {
                treasure[left] = true;
            } else if (right <= n && allowed[right]) {
                treasure[right] = true;
            } else {
                possible = false;
            }
        }

        bool hasTreasure = false;
        for (int i = 1; i <= n; ++i) hasTreasure = hasTreasure || treasure[i];
        if (!hasTreasure && possible) {
            // This only happens when every entry was destroyed.
            treasure[1] = true;
        }

        if (!possible) {
            cout << "-1\n";
        } else {
            for (int i = 1; i <= n; ++i) cout << (treasure[i] ? '1' : '0');
            cout << '\n';
        }
    }
    return 0;
}
