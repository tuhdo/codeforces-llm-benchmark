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
        vector<int> b(n);
        for (int &x : b) cin >> x;

        // A known distance d at i forbids treasure at every position
        // strictly closer than d. Use a difference array to mark these ranges.
        vector<int> diff(n + 1, 0);
        for (int i = 0; i < n; ++i) {
            if (b[i] < 0) continue;
            int left = max(0, i - b[i] + 1);
            int right = min(n - 1, i + b[i] - 1);
            if (left <= right) {
                ++diff[left];
                --diff[right + 1];
            }
        }

        vector<char> forbidden(n, false);
        int active = 0;
        for (int i = 0; i < n; ++i) {
            active += diff[i];
            forbidden[i] = (active > 0);
        }

        vector<char> treasure(n, false);
        bool possible = true;
        for (int i = 0; i < n; ++i) {
            if (b[i] < 0) continue;
            bool satisfied = false;
            int left = i - b[i];
            int right = i + b[i];
            if (left >= 0 && !forbidden[left]) {
                treasure[left] = true;
                satisfied = true;
            }
            if (right < n && !forbidden[right]) {
                treasure[right] = true;
                satisfied = true;
            }
            if (!satisfied) possible = false;
        }

        if (!possible) {
            cout << -1 << '\n';
            continue;
        }
        if (none_of(treasure.begin(), treasure.end(), [](char x) { return x; }))
            treasure[0] = true;

        for (char x : treasure) cout << (x ? '1' : '0');
        cout << '\n';
    }
    return 0;
}
