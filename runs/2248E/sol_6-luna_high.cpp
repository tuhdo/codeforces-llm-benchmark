#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        long long n, d;
        int m;
        cin >> n >> m >> d;

        vector<long long> p(m), r(m), pref(m + 1, 0);
        for (int i = 0; i < m; ++i) {
            cin >> p[i] >> r[i];
            pref[i + 1] = pref[i] + r[i];
        }

        const long long cycleReward = pref[m];
        auto score = [&](long long len) -> long long {
            long long cycles = len / n;
            long long rem = len % n;
            int count = upper_bound(p.begin(), p.end(), rem) - p.begin();
            return len * d + cycles * cycleReward + pref[count];
        };

        bool possible = false;
        vector<long long> atReward(m);
        for (int i = 0; i < m; ++i) atReward[i] = score(p[i]);

        for (int i = 0; i < m && !possible; ++i) {
            for (int j = 0; j < m; ++j) {
                long long combined = p[i] + p[j] + 1;
                if (atReward[i] + atReward[j] > score(combined)) {
                    possible = true;
                    break;
                }
            }
        }

        cout << (possible ? "YES\n" : "NO\n");
    }
    return 0;
}
