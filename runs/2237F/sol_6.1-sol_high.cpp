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
        int n, m;
        cin >> n >> m;

        // A valid array starts with 1, ends with m, and each adjacent pair
        // either increases by one, has its second value 1, or its first m.
        // dp[x] maximizes unchanged elements in a prefix ending with x.
        // Transitions into 1 use max(dp); transitions into x > 1 use
        // max(dp[x - 1], dp[m]). A matching input value adds one.
        constexpr int impossible = -1'000'000'000;
        vector<int> dp(m, impossible);
        int a;
        cin >> a;
        dp[0] = (a == 1);
        int best = dp[0];
        int floor_value = impossible;
        int head = 0;

        for (int i = 1; i < n; ++i) {
            cin >> a;
            int last = (head == 0 ? m - 1 : head - 1);
            int previous_m = max(dp[last], floor_value);

            // Rotate so that old x becomes new x + 1. The old m slot
            // becomes 1. Each stored entry represents max(entry, floor).
            head = last;
            dp[head] = best;
            floor_value = previous_m;

            int matching = head + a - 1;
            if (matching >= m) matching -= m;
            dp[matching] = max(dp[matching], floor_value) + 1;
            best = max(best, dp[matching]);
        }

        int last = (head == 0 ? m - 1 : head - 1);
        int unchanged = max(dp[last], floor_value);
        cout << n - unchanged << '\n';
    }
}
