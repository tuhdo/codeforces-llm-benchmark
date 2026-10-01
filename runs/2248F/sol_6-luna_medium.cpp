#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    if (!(cin >> T)) return 0;
    while (T--) {
        int n, m, k;
        cin >> n >> m >> k;
        vector<vector<long long>> a(n, vector<long long>(m));
        vector<long long> rs(n), cs(m);
        for (int i = 0; i < n; ++i) for (int j = 0; j < m; ++j) {
            cin >> a[i][j]; rs[i] += a[i][j]; cs[j] += a[i][j];
        }
        if (n == 1 && m == 1) { cout << 0 << '\n'; continue; }
        long long gain = n + m - 3;
        if (gain <= 0) { cout << -1 << '\n'; continue; }
        vector<long long> need;
        need.reserve((size_t)n * m);
        for (int i = 0; i < n; ++i) for (int j = 0; j < m; ++j) {
            long long margin = a[i][j] - (rs[i] - a[i][j]) - (cs[j] - a[i][j]);
            long long q = margin >= 0 ? 0 : (-margin + gain - 1) / gain;
            need.push_back(q);
        }
        nth_element(need.begin(), need.begin() + (k - 1), need.end());
        cout << need[k - 1] << '\n';
    }
    return 0;
}
