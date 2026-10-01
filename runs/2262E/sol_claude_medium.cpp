#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
ll M;
ll pw(ll b, ll e) {
    ll r = 1;
    b %= M;
    while (e) {
        if (e & 1) r = r * b % M;
        b = b * b % M;
        e >>= 1;
    }
    return r;
}
int main() {
    int t;
    scanf("%d", &t);
    while (t--) {
        int n;
        scanf("%d %lld", &n, &M);
        int N = 2 * n + 2;
        vector<vector<int>> C(N + 1, vector<int>(N + 1, 0));
        for (int i = 0; i <= N; i++) {
            C[i][0] = 1 % M;
            for (int j = 1; j <= i; j++)
                C[i][j] = (C[i - 1][j - 1] + (j <= i - 1 ? C[i - 1][j] : 0)) % M;
        }
        vector<ll> H(n + 1);
        for (int a = 0; a <= n; a++) {
            ll c = (C[2 * a][a] - (a + 1 <= 2 * a ? C[2 * a][a + 1] : 0) + M) % M;
            H[a] = c * c % M;
        }
        vector<ll> P = H, F(n + 1);
        for (int m = 0; m <= n; m++) {
            // P = H^{m+1}, truncated to degree n-m
            ll h = P[n - m];
            F[m] = (ll)C[2 * n][m] * h % M * pw(m + 1, M - 2) % M;
            if (m < n) {
                int lim = n - m - 1;
                vector<ll> Q(lim + 1, 0);
                for (int i = 0; i <= lim; i++) {
                    if (!P[i]) continue;
                    for (int j = 0; i + j <= lim; j++)
                        Q[i + j] = (Q[i + j] + P[i] * H[j]) % M;
                }
                P = Q;
            }
        }
        for (int k = 0; k <= n; k++) {
            ll r = 0;
            for (int m = k; m <= n; m++) {
                ll v = (ll)C[m][k] * F[m] % M;
                if ((m - k) & 1) r = (r - v + M) % M;
                else r = (r + v) % M;
            }
            printf("%lld ", r);
        }
        puts("");
    }
}
