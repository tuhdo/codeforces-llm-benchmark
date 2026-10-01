#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
using i128 = __int128_t;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;

    while (t--) {
        int n;
        cin >> n;

        vector<pair<int64, int64>> items(n);
        int64 total = 0;
        int64 maximum = 0;

        for (auto &[x, y] : items) {
            cin >> x >> y;
            total += y;
            maximum = max(maximum, x);
        }

        sort(items.begin(), items.end());

        auto can_make = [&](int64 target) -> bool {
            i128 carry = 0;
            i128 used = 0;
            int64 current = target - 1;

            auto advance_missing = [&](int64 gap) -> bool {
                // Each absent level changes carry to 2 * carry + 1.
                // Equivalently, carry + 1 is multiplied by 2^gap.
                if (gap == 0) return true;
                if (gap >= 63) return false;

                i128 value = (carry + 1) * (static_cast<i128>(1) << gap);
                if (value > static_cast<i128>(total) + 1) return false;
                carry = value - 1;
                return true;
            };

            for (int i = n - 1; i >= 0; --i) {
                const auto [x, y] = items[i];
                if (x == 0) break;

                int64 gap = current - x;
                if (!advance_missing(gap)) return false;

                i128 demand = carry + 1;
                i128 take = min<i128>(demand, y);
                used += take;
                carry += demand - take;

                if (carry > total) return false;
                current = x - 1;
            }

            if (!advance_missing(max<int64>(0, current))) return false;

            // Every unused initial element can be turned into a zero.
            return carry + 1 <= static_cast<i128>(total) - used;
        };

        int64 answer = maximum;

        // Beyond the largest initial value, feasibility is monotone.
        // Each extra missing level doubles the carry, so 64 steps are enough
        // for the given total number of elements.
        for (int extra = 1; extra <= 64; ++extra) {
            int64 candidate = maximum + extra;
            if (!can_make(candidate)) break;
            answer = candidate;
        }

        cout << answer << '\n';
    }

    return 0;
}
