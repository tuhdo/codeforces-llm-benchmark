#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        vector<int> a(n), b(n);
        for (int& x : a) {
            cin >> x;
            --x;
        }
        for (int& x : b) {
            cin >> x;
            --x;
        }

        bool possible = true;
        int remaining = 0;
        int start = 0;
        for (int i = 0; i < n; ++i) {
            if (a[i] > b[i]) possible = false;
            if (a[i] < b[i]) {
                if (remaining == 0) start = i;
                ++remaining;
            }
        }
        if (!possible) {
            cout << "-1\n";
            continue;
        }

        vector<int> operations;
        int position = start;
        while (remaining > 0) {
            if (a[position] < b[position]) {
                // If the edge is already forward, or its destination is done,
                // advancing it preserves every unfinished vertex's reachability.
                while (a[position] < b[position] &&
                       (a[position] >= position || a[a[position]] == b[a[position]])) {
                    ++a[position];
                    operations.push_back(1);
                }
                if (a[position] == b[position]) --remaining;
            }

            if (remaining == 0 || a[position] == position) break;
            operations.push_back(2);
            position = a[position];
        }

        if (remaining != 0) {
            cout << "-1\n";
            continue;
        }
        cout << operations.size() << ' ' << start + 1 << '\n';
        for (int op : operations) cout << op << ' ';
        cout << '\n';
    }
}
