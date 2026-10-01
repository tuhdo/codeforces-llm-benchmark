#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

static bool canMake(int64 target, const vector<pair<int64, int64>>& values,
                    int64 totalCount) {
    const int64 cap = totalCount + 1;
    int64 need = 1;
    int64 extraZeros = 0;
    int64 nextValue = target - 1;

    for (auto it = values.rbegin(); it != values.rend(); ++it) {
        const auto [x, count] = *it;
        if (x == 0 || x >= target) continue;

        const int64 gap = nextValue - x;
        if (gap > 0) {
            if (gap >= 63 || need > (cap >> gap)) need = cap;
            else need <<= gap;
        }

        if (count < need) {
            const int64 shortage = need - count;
            need = min(cap, need + shortage);
        } else {
            extraZeros = min(cap, extraZeros + (count - need));
        }
        nextValue = x - 1;
    }

    if (nextValue > 0) {
        if (nextValue >= 63 || need > (cap >> nextValue)) need = cap;
        else need <<= nextValue;
    }

    int64 zeroCount = 0;
    for (const auto& [x, count] : values) {
        if (x == 0) {
            zeroCount = count;
            break;
        }
    }
    return min(cap, zeroCount + extraZeros) >= need;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<pair<int64, int64>> values(n);
        int64 maxValue = 0;
        int64 totalCount = 0;
        for (auto& [x, count] : values) {
            cin >> x >> count;
            maxValue = max(maxValue, x);
            totalCount += count;
        }
        sort(values.begin(), values.end());

        int64 low = maxValue;
        int64 high = maxValue + 60;
        while (low < high) {
            const int64 mid = low + (high - low + 1) / 2;
            if (canMake(mid, values, totalCount)) low = mid;
            else high = mid - 1;
        }

        const int64 mexoramax = low;
        cout << mexoramax << '\n';
    }
    return 0;
}
