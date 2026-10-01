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

        vector<long long> positions(m + 1, 0);
        vector<long long> prefix(m + 1, 0);
        for (int i = 1; i <= m; ++i) {
            long long reward;
            cin >> positions[i] >> reward;
            prefix[i] = prefix[i - 1] + reward;
        }

        // Full n-step cycles can be removed from either run.
        int candidates = m + 1;
        if (m > 0 && positions[m] == n) {
            --candidates;
        }

        bool possible = false;
        for (int i = 0; i < candidates && !possible; ++i) {
            for (int j = i; j < candidates; ++j) {
                long long length = positions[i] + positions[j] + 1;
                long long baseline = (length / n) * prefix[m];
                long long remainder = length % n;
                int index = int(upper_bound(positions.begin(), positions.end(),
                                            remainder) - positions.begin()) - 1;
                baseline += prefix[index];

                // Exactly one zero loses d relative to the all-ones array.
                if (prefix[i] + prefix[j] > baseline + d) {
                    possible = true;
                    break;
                }
            }
        }

        cout << (possible ? "YES\n" : "NO\n");
    }
    return 0;
}
