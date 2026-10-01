#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
using i128 = __int128_t;

struct Item {
    int64 value;
    int64 count;
};

static constexpr i128 LIMIT = (i128)4'000'000'000'000'000'000LL;

// Applying g consecutive missing levels changes u to 2^g * (u + 1) - 1.
static bool apply_missing(i128 &u, int64 g) {
    while (g-- > 0) {
        if (u > (LIMIT - 1) / 2) {
            u = LIMIT;
            return false;
        }
        u = 2 * u + 1;
    }
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;

        vector<Item> items(n);
        int64 total = 0;
        int64 zero_count = 0;
        int64 maximum = 0;
        for (auto &item : items) {
            cin >> item.value >> item.count;
            total += item.count;
            maximum = max(maximum, item.value);
            if (item.value == 0) zero_count = item.count;
        }
        sort(items.begin(), items.end(), [](const Item &a, const Item &b) {
            return a.value < b.value;
        });

        auto feasible = [&](int64 target) {
            // u is the number of additional copies required at every lower
            // level because higher missing levels had to be generated.
            i128 u = 0;
            int64 unused_positive = total - zero_count;
            int64 previous = target;

            for (int i = n - 1; i >= 0; --i) {
                const auto [value, count] = items[i];
                if (value == 0 || value >= target) continue;

                if (!apply_missing(u, previous - value - 1)) return false;

                i128 required = u + 1;
                i128 used = min<i128>(count, required);
                // Any unmet demand must itself be generated, adding the same
                // amount to the demand at every lower level.
                u += required - used;
                unused_positive -= (int64)used;
                previous = value;
            }

            if (!apply_missing(u, previous - 1)) return false;
            i128 required_zero = u + 1;
            return required_zero <= (i128)zero_count + unused_positive;
        };

        int64 answer = maximum;
        // A missing level doubles the required supply, so with at most
        // 2e14 elements, at most a few dozen levels beyond maximum matter.
        for (int64 target = maximum + 1; target <= maximum + 64; ++target) {
            if (!feasible(target)) break;
            answer = target;
        }
        cout << answer << '\n';
    }
    return 0;
}
