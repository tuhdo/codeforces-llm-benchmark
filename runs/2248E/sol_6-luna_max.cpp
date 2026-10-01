#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

// Maximum bonus contained in any cyclic interval of `length` positions.
static int64 max_cyclic_window(
    int64 length,
    int64 n,
    const vector<int64>& p,
    const vector<int64>& reward
) {
    const int m = static_cast<int>(p.size());
    if (m == 0) return 0;

    vector<int64> position(2 * m);
    vector<int64> prefix(2 * m + 1, 0);
    for (int i = 0; i < 2 * m; ++i) {
        const int source = i % m;
        position[i] = p[source] + (i >= m ? n : 0);
        prefix[i + 1] = prefix[i] + reward[source];
    }

    int64 best = 0;
    int right = 0;
    for (int left = 0; left < m; ++left) {
        right = max(right, left);
        while (right < left + m && position[right] - position[left] < length) {
            ++right;
        }
        best = max(best, prefix[right] - prefix[left]);
    }
    return best;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int64 n, d;
        int m;
        cin >> n >> m >> d;

        vector<int64> p(m), reward(m), prefix(m + 1, 0);
        for (int i = 0; i < m; ++i) {
            cin >> p[i] >> reward[i];
            prefix[i + 1] = prefix[i] + reward[i];
        }
        const int64 total_bonus = prefix[m];

        bool possible = false;
        if (n >= 3) {
            vector<int64> candidates{1};
            for (int i = 0; i < m; ++i) {
                if (p[i] <= n - 2 && p[i] != 1) {
                    candidates.push_back(p[i]);
                }
            }

            for (int64 r : candidates) {
                const int count = upper_bound(p.begin(), p.end(), r) - p.begin();
                const int64 prefix_bonus = prefix[count];

                // A run of length r is compared with an all-ones interval of
                // length r + 1. Its minimum bonus is total_bonus minus the
                // maximum bonus in the complementary cyclic window.
                const int64 complement_length = n - r - 1;
                const int64 max_complement = max_cyclic_window(
                    complement_length, n, p, reward
                );
                const int64 min_baseline_bonus = total_bonus - max_complement;

                if (prefix_bonus > d + min_baseline_bonus) {
                    possible = true;
                    break;
                }
            }
        }

        cout << (possible ? "YES" : "NO") << '\n';
    }
    return 0;
}
