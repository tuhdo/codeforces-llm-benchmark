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
        vector<int> a(n);
        for (int &x : a) cin >> x;
        sort(a.begin(), a.end());

        auto possible = [&](int seconds) {
            int large = max(0, seconds - 30);
            int remaining = max(0, n - large);
            priority_queue<long long> requirements;
            for (int i = 0; i < remaining; ++i)
                requirements.push(a[i]);

            for (int bit = min(seconds, 30) - 1;
                 bit >= 0 && !requirements.empty(); --bit) {
                long long need = requirements.top();
                requirements.pop();
                need -= 1LL << bit;
                if (need > 0) requirements.push(need);
            }
            return requirements.empty();
        };

        int low = n, high = n + 30;
        while (low < high) {
            int mid = low + (high - low) / 2;
            if (possible(mid)) high = mid;
            else low = mid + 1;
        }
        cout << low << '\n';
    }
}
