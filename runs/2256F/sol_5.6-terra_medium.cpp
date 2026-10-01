#include <bits/stdc++.h>
using namespace std;

static bool possible(const vector<long long>& a, int seconds) {
    priority_queue<long long> remaining(a.begin(), a.end());

    for (int bit = seconds - 1; bit >= 0; --bit) {
        long long need = remaining.top();
        remaining.pop();

        long long power = bit >= 60 ? LLONG_MAX : (1LL << bit);
        if (need > power) {
            remaining.push(need - power);
        } else {
            remaining.push(0);
        }
    }

    return remaining.top() == 0;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        vector<long long> a(n);
        for (long long& x : a) cin >> x;

        int low = n, high = n + 30;
        while (low < high) {
            int mid = low + (high - low) / 2;
            if (possible(a, mid)) high = mid;
            else low = mid + 1;
        }
        cout << low << '\n';
    }
}
