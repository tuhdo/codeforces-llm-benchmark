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
        vector<int> a(n);
        for (int &x : a) cin >> x;
        vector<vector<int>> g(n);
        for (int i = 1; i < n; ++i) {
            int u, v;
            cin >> u >> v;
            --u; --v;
            g[u].push_back(v);
            g[v].push_back(u);
        }

        vector<int> parent(n, -1), order;
        order.reserve(n);
        order.push_back(0);
        for (int i = 0; i < (int)order.size(); ++i) {
            int v = order[i];
            for (int u : g[v]) if (u != parent[v]) {
                parent[u] = v;
                order.push_back(u);
            }
        }

        vector<int> sz(n), zeros(n);
        vector<int> fixed(n);
        vector<long long> base(n);
        vector<int> neg(n), mid(n), pos(n);

        for (int it = n - 1; it >= 0; --it) {
            int v = order[it];
            sz[v] = 1;
            zeros[v] = (a[v] == 0);
            fixed[v] = (a[v] == 0 ? 0 : a[v]);
            base[v] = 0;
            neg[v] = mid[v] = pos[v] = 0;

            for (int u : g[v]) if (parent[u] == v) {
                sz[v] += sz[u];
                zeros[v] += zeros[u];
                fixed[v] += fixed[u];
                base[v] += base[u];
                neg[v] += neg[u];
                mid[v] += mid[u];
                pos[v] += pos[u];
            }

            int parity = sz[v] & 1;
            int L = (fixed[v] - zeros[v] - parity) / 2;
            int R = (fixed[v] + zeros[v] - parity) / 2;

            auto value = [&](int k) -> int { return abs(2 * k + parity); };
            base[v] += value(L);

            // Add the slopes of |2k + parity| for k=L+1,...,R.
            // For parity 0: slopes are -2 up to k=0, then +2.
            // For parity 1: slopes are -2 up to k=-1, one zero slope
            // at k=0, then +2.
            if (parity == 0) {
                int cntNeg = max(0, min(R, 0) - L);
                int cntPos = max(0, R - max(L, 0));
                neg[v] += cntNeg;
                pos[v] += cntPos;
            } else {
                int cntNeg = max(0, min(R, -1) - L);
                int cntMid = (L < 0 && R >= 0) ? 1 : 0;
                int cntPos = max(0, R - max(L, 0));
                neg[v] += cntNeg;
                mid[v] += cntMid;
                pos[v] += cntPos;
            }

            if (v == 0) {
                cout << base[v] - 2LL * neg[v] << '\n';
            }
        }
    }
}
