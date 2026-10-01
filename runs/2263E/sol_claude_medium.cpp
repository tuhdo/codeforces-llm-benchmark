#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll MOD = 1e9 + 7;

ll fact(int x) {
    ll r = 1;
    for (int i = 2; i <= x; i++) r = r * i % MOD;
    return r;
}

int main() {
    int t;
    scanf("%d", &t);
    while (t--) {
        int n;
        scanf("%d", &n);
        vector<ll> a(n);
        for (auto &x : a) scanf("%lld", &x);
        ll M = a[n - 1];
        int s = n / 2;
        set<ll> cand;
        cand.insert(a[1]);
        ll d = (n % 2 == 0) ? s - 1 : s;
        if (d > 0 && (M - a[1]) % d == 0) cand.insert((M - a[1]) / d);
        int good = 0;
        for (ll dl : cand) {
            if (dl <= 0) continue;
            vector<ll> c(n);
            for (int k = 1; k <= n; k++) {
                if (k & 1) c[k - 1] = M - (ll)((k - 1) / 2) * dl;
                else c[k - 1] = (ll)(k / 2 - 1) * dl;
            }
            sort(c.begin(), c.end());
            if (c == a) good++;
        }
        ll ans = 0;
        if (good) {
            if (n % 2 == 0) ans = fact(s) * fact(s - 2) % MOD;
            else ans = fact(s) * fact(s - 1) % MOD;
            ans = ans * good % MOD;
        }
        printf("%lld\n", ans);
    }
}
