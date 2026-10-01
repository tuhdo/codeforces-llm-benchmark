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
        for (int& value : a) cin >> value;
        string s;
        cin >> s;

        // For every zero, store how many ones are before it.  These values
        // are nondecreasing and their sum is exactly the inversion count.
        deque<long long> positive;
        long long inversions = 0;
        long long ones = 0;
        for (int value : a) {
            if (value == 1) {
                ++ones;
            } else {
                inversions += ones;
                if (ones > 0) positive.push_back(ones);
            }
        }

        long long removed_columns = 0;
        cout << inversions;
        for (char operation : s) {
            long long decrease;
            if (operation == '1') {
                // A bubble moves every zero having a preceding one across
                // one such one: all positive values lose one.
                decrease = static_cast<long long>(positive.size());
                ++removed_columns;
                while (!positive.empty() && positive.front() <= removed_columns)
                    positive.pop_front();
            } else {
                // A reverse bubble drops the largest value and shifts the
                // remaining values right, so the loss is that largest value.
                decrease = positive.empty() ? 0 : positive.back() - removed_columns;
                if (!positive.empty()) positive.pop_back();
            }
            inversions -= decrease;
            cout << ' ' << inversions;
        }
        cout << '\n';
    }
}
