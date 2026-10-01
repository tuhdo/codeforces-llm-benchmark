#include <bits/stdc++.h>
using namespace std;

static bool possible(const vector<long long>& a, int k) {
    priority_queue<long long> need;
    for (long long x : a) need.push(x);

    for (int bit = k - 1; bit >= 0 && !need.empty(); --bit) {
        long long weight = 1LL << bit;
        long long x = need.top();
        need.pop();
        x -= weight;
        if (x > 0) need.push(x);
    }
    return need.empty();
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
        for (auto& x : a) cin >> x;

        int lo = 0, hi = 32;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (possible(a, mid)) hi = mid;
            else lo = mid + 1;
        }
        cout << lo << '\n';
    }
}
