#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCount;
    cin >> testCount;

    while (testCount--) {
        long long n, d;
        int m;
        cin >> n >> m >> d;

        vector<long long> position(m), reward(m);
        for (int i = 0; i < m; ++i) {
            cin >> position[i] >> reward[i];
        }

        if (m == 0 || n == 1) {
            cout << "NO\n";
            continue;
        }

        // Duplicate the circle. A window after position[j] contains rewards
        // with positions in (position[j], position[j] + length].
        vector<long long> doubledPosition(2 * m), doubledPrefix(2 * m + 1, 0);
        for (int i = 0; i < 2 * m; ++i) {
            doubledPosition[i] = position[i % m] + (i >= m ? n : 0);
            doubledPrefix[i + 1] = doubledPrefix[i] + reward[i % m];
        }

        long long prefixReward = 0;
        bool possible = false;

        for (int i = 0; i < m; ++i) {
            prefixReward += reward[i];

            // A useful run has remainder s in [1, n - 1].  It is enough to
            // check s at a reward position because the other term only grows
            // between consecutive reward positions.
            if (position[i] == n) {
                break;
            }

            const long long length = position[i] + 1;
            long long minimumWindowReward = LLONG_MAX;
            int right = 1;

            for (int left = 0; left < m; ++left) {
                right = max(right, left + 1);
                while (right < left + m + 1 &&
                       doubledPosition[right] <= doubledPosition[left] + length) {
                    ++right;
                }

                minimumWindowReward = min(
                    minimumWindowReward,
                    doubledPrefix[right] - doubledPrefix[left + 1]);
            }

            if (prefixReward - minimumWindowReward > d) {
                possible = true;
                break;
            }
        }

        cout << (possible ? "YES\n" : "NO\n");
    }

    return 0;
}
