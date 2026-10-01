#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        long long x;
        cin >> n >> x;
        vector<int> m(n);
        vector<vector<long long>> a(n), b(n);
        for (int i = 0; i < n; ++i) {
            cin >> m[i];
            a[i].resize(m[i]);
            b[i].resize(m[i]);
            for (auto &v : a[i]) cin >> v;
            for (auto &v : b[i]) cin >> v;
        }

        vector<int> done(n);
        long long money = x;
        bool changed = true;
        while (changed) {
            changed = false;
            for (int i = 0; i < n; ++i) {
                while (done[i] < m[i] && a[i][done[i]] <= money &&
                       b[i][done[i]] >= a[i][done[i]]) {
                    money += b[i][done[i]] - a[i][done[i]];
                    ++done[i];
                    changed = true;
                }
            }
        }

        int best = 0, idx = 1;
        for (int i = 0; i < n; ++i) {
            long long cur = money;
            int h = 0;
            for (int j = 0; j < m[i]; ++j) {
                if (a[i][j] > cur) break;
                cur += b[i][j] - a[i][j];
                ++h;
            }
            if (h > best) {
                best = h;
                idx = i + 1;
            }
        }
        cout << best << ' ' << idx << '\n';
    }
}
