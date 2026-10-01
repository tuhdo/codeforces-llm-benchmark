#include <algorithm>
#include <functional>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

bool feasible(const vector<int>& a, int bits, int seconds) {
    int removed = max(0, seconds - bits);
    int remaining = static_cast<int>(a.size()) - removed;
    if (remaining <= 0) return true;

    priority_queue<int> requirements(
        less<int>(), vector<int>(a.begin(), a.begin() + remaining));

    for (int bit = min(bits, seconds) - 1; bit >= 0; --bit) {
        if (requirements.empty()) return true;
        int need = requirements.top();
        requirements.pop();
        int weight = 1 << bit;
        if (need > weight) requirements.push(need - weight);
    }
    return requirements.empty();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<int> a(n);
        for (int& value : a) cin >> value;
        sort(a.begin(), a.end());

        int bits = 0;
        for (int value = a.back(); value > 0; value >>= 1) ++bits;

        int low = n;
        int high = n + bits - 1;
        while (low < high) {
            int mid = low + (high - low) / 2;
            if (feasible(a, bits, mid)) high = mid;
            else low = mid + 1;
        }
        cout << low << '\n';
    }
}
