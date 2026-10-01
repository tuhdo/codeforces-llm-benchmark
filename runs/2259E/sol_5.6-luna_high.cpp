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

        vector<int> diff(n + 1, 0);
        for (int i = 0; i < n; ++i) {
            if (b[i] <= 0) continue;

            int left = max(0, i - b[i] + 1);
            int right = min(n - 1, i + b[i] - 1);
            ++diff[left];
            --diff[right + 1];
        }

        vector<char> available(n);
        int covered = 0;
        for (int i = 0; i < n; ++i) {
            covered += diff[i];
            available[i] = (covered == 0);
        }

        bool ok = true;
        for (int i = 0; i < n && ok; ++i) {
            if (b[i] == -1) continue;
            if (b[i] == 0) {
                ok = available[i];
                continue;
            }

            bool witness = false;
            if (i - b[i] >= 0) witness = witness || available[i - b[i]];
            if (i + b[i] < n) witness = witness || available[i + b[i]];
            ok = witness;
        }

        if (!ok) {
            cout << "-1\n";
        } else {
            for (char canUse : available) cout << (canUse ? '1' : '0');
            cout << '\n';
        }
    }
    return 0;
}
