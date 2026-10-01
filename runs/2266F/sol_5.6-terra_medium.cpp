#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;

        vector<pair<int64, int64>> a(n);
        int64 total = 0, maximum = 0;
        for (auto& [x, y] : a) {
            cin >> x >> y;
            total += y;
            maximum = max(maximum, x);
        }

        int lg = 0;
        while ((1LL << lg) <= total) ++lg;
        // More than n missing levels cannot be bridged with only n distinct
        // initially available values; after that, demand doubles per level.
        int64 bound = n + lg + 2LL;
        if (maximum > bound) {
            cout << maximum << '\n';
            continue;
        }

        int limit = static_cast<int>(bound);
        vector<int64> count(limit + 1, 0);
        for (auto [x, y] : a) {
            if (x <= limit) count[static_cast<int>(x)] = y;
        }

        auto can_make = [&](int k) {
            if (count[k] > 0) return true;

            // `need` is the number of requests for the current value caused
            // by unresolved larger requests. `used` counts existing nonzero
            // elements kept to answer such requests.
            int64 need = 1, used = 0;
            const int64 cap = total + 1;
            for (int i = k - 1; i >= 1; --i) {
                int64 take = min(need, count[i]);
                used = min(cap, used + take);
                int64 missing = need - take;
                need = min(cap, need + missing);
            }
            // Requests for zero can be satisfied by any remaining element,
            // since a singleton nonzero element can be replaced by zero.
            return need + used <= total;
        };

        int lo = 0, hi = limit;
        while (lo < hi) {
            int mid = lo + (hi - lo + 1) / 2;
            if (can_make(mid)) lo = mid;
            else hi = mid - 1;
        }
        cout << lo << '\n';
    }
}
