#include <algorithm>
#include <iostream>
#include <vector>

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

        vector<long long> p(m), r(m);
        vector<long long> prefix(m + 1, 0);
        long long total_reward = 0;
        for (int i = 0; i < m; ++i) {
            cin >> p[i] >> r[i];
            total_reward += r[i];
            prefix[i + 1] = prefix[i] + r[i];
        }

        if (n == 1) {
            cout << "NO\n";
            continue;
        }

        auto bonus = [&](long long length) {
            auto it = upper_bound(p.begin(), p.end(), length);
            return prefix[it - p.begin()];
        };

        vector<long long> candidates{1};
        for (long long position : p) {
            if (position < n) {
                candidates.push_back(position);
            }
        }

        bool possible = false;
        for (long long a : candidates) {
            long long bonus_a = bonus(a);
            for (long long b : candidates) {
                long long length = a + b + 1;
                long long all_ones_bonus = (length / n) * total_reward
                    + bonus(length % n);

                if (bonus_a + bonus(b) > all_ones_bonus + d) {
                    possible = true;
                    break;
                }
            }
            if (possible) {
                break;
            }
        }

        cout << (possible ? "YES\n" : "NO\n");
    }

    return 0;
}
