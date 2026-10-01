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
        vector<int> counts;
        counts.reserve(n);
        int ones = 0;
        long long inversions = 0;
        for (int i = 0; i < n; ++i) {
            int bit;
            cin >> bit;
            if (bit == 1) {
                ++ones;
            } else {
                counts.push_back(ones);
                inversions += ones;
            }
        }
        string s;
        cin >> s;

        int left = 0;
        int right = static_cast<int>(counts.size()) - 1;
        int offset = 0;
        while (left <= right && counts[left] == 0) {
            ++left;
        }

        cout << inversions;
        for (char operation : s) {
            if (left <= right) {
                if (operation == '1') {
                    // Every zero with a preceding one crosses exactly one one.
                    inversions -= right - left + 1;
                    ++offset;
                    while (left <= right && counts[left] <= offset) {
                        ++left;
                    }
                } else {
                    // The counts shift right, so the last count disappears.
                    inversions -= counts[right] - offset;
                    --right;
                }
            }
            cout << ' ' << inversions;
        }
        cout << '\n';
    }
    return 0;
}
