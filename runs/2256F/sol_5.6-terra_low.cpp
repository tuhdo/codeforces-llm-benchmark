#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

static bool possible(const vector<long long>& a, int operations) {
    priority_queue<long long> remaining(a.begin(), a.end());

    // Process the available weights from largest to smallest.  Giving the
    // current weight to the largest unmet requirement is always optimal.
    for (int bit = operations - 1; bit >= 0; --bit) {
        if (remaining.top() == 0) return true;

        const long long weight = bit >= 30 ? (1LL << 60) : (1LL << bit);
        long long need = remaining.top();
        remaining.pop();
        remaining.push(max(0LL, need - weight));
    }
    return remaining.top() == 0;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<long long> a(n);
        for (long long& x : a) cin >> x;

        int low = n;
        int high = n + 30;
        while (low < high) {
            int mid = low + (high - low) / 2;
            if (possible(a, mid)) high = mid;
            else low = mid + 1;
        }
        cout << low << '\n';
    }
}
