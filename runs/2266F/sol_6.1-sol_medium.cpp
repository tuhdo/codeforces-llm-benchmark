#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        const int limit = n + 50;
        vector<long long> count(limit, 0);
        long long total = 0, largest = 0;
        for (int i = 0; i < n; ++i) {
            long long x, y;
            cin >> x >> y;
            total += y;
            largest = max(largest, x);
            if (x < limit) count[x] = y;
        }

        auto possible = [&](int k) {
            long long need = 1;
            long long remaining = total;
            for (int i = k - 1; i >= 1; --i) {
                long long used = min(count[i], need);
                remaining -= used;
                need += need - used;
                if (need > remaining) return false;
            }
            return need <= remaining;
        };

        int low = 0, high = limit;
        while (high - low > 1) {
            int mid = low + (high - low) / 2;
            if (possible(mid)) low = mid;
            else high = mid;
        }
        cout << max(largest, static_cast<long long>(low)) << '\n';
    }
}
