#include <bits/stdc++.h>

using namespace std;

using int64 = long long;

struct CustomHash {
    static uint64_t splitmix64(uint64_t x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    size_t operator()(uint64_t x) const {
        static const uint64_t fixed_random =
            chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + fixed_random);
    }
};

// Can we have one copy of every value in [0, mex) simultaneously?
static bool can_make_prefix(
    int64 mex,
    int64 total,
    const unordered_map<int64, int64, CustomHash>& count
) {
    if (mex <= 1) {
        return true;
    }

    // `missing_above` is the number of already-expanded larger values that
    // each smaller value is required to supply.
    int64 missing_above = 0;
    int64 used_directly = 0;

    for (int64 value = mex - 1; value >= 2; --value) {
        const int64 needed = missing_above + 1;
        const auto it = count.find(value);
        const int64 available = (it == count.end() ? 0 : it->second);
        const int64 used_here = min(needed, available);

        used_directly += used_here;
        missing_above += needed - used_here;

        // Every unresolved construction needs both a 0 and a 1 eventually.
        // More iterations can only increase this demand and consume more items.
        if (missing_above + 1 > (total - used_directly) / 2) {
            return false;
        }
    }

    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int test_count;
    cin >> test_count;

    while (test_count--) {
        int n;
        cin >> n;

        unordered_map<int64, int64, CustomHash> count;
        count.reserve(static_cast<size_t>(n) * 2 + 1);
        count.max_load_factor(0.7F);

        int64 total = 0;
        int64 largest_initial = 0;

        for (int i = 0; i < n; ++i) {
            int64 x, y;
            cin >> x >> y;
            count[x] = y;
            total += y;
            largest_initial = max(largest_initial, x);
        }

        int64 low = 1;
        int64 high = total;
        while (low < high) {
            const int64 mid = low + (high - low + 1) / 2;
            if (can_make_prefix(mid, total, count)) {
                low = mid;
            } else {
                high = mid - 1;
            }
        }

        cout << max(largest_initial, low) << '\n';
    }
}
