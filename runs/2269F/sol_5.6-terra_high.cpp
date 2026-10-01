#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        vector<int> p(n + 2), nextGreater(n + 2, n + 1), firstParent(n + 2, n + 1);
        vector<int64> dp(n + 2);
        vector<vector<int>> removeAt(n + 2);
        vector<char> marked(n + 2);

        for (int i = 1; i <= n; ++i) cin >> p[i];

        vector<int> decreasing;
        for (int i = n; i >= 1; --i) {
            while (!decreasing.empty() && p[decreasing.back()] < p[i]) {
                decreasing.pop_back();
            }
            nextGreater[i] = decreasing.empty() ? n + 1 : decreasing.back();
            firstParent[nextGreater[i]] = i;
            decreasing.push_back(i);
        }

        auto solveBlock = [&](int left, int right) {
            dp[right] = right - left;
            int rightmost = right;
            int markedCount = 0;

            if (firstParent[right] != n + 1) {
                removeAt[firstParent[right]].push_back(right);
                marked[right] = true;
                ++markedCount;
            }

            for (int i = right - 1; i >= left; --i) {
                while (!marked[rightmost]) --rightmost;

                if (rightmost == nextGreater[i]) {
                    dp[i] = dp[nextGreater[i]] + right - i - 1;
                } else {
                    int distanceTwo = nextGreater[i] - i - 1
                        + (nextGreater[nextGreater[i]] != n + 1)
                        + markedCount - 1
                        - (nextGreater[nextGreater[i]] != n + 1
                           && firstParent[nextGreater[nextGreater[i]]] <= i);
                    dp[i] = dp[rightmost] + 2LL * (right - i) - 2 - distanceTwo;
                }

                for (int position : removeAt[i]) {
                    marked[position] = false;
                    --markedCount;
                }
                if (firstParent[i] != n + 1) {
                    removeAt[firstParent[i]].push_back(i);
                    marked[i] = true;
                    ++markedCount;
                }
            }

            for (int i = left; i <= right; ++i) dp[i] += left - 1;
        };

        int left = 1;
        for (int right = 1; right <= n; ++right) {
            if (nextGreater[right] == n + 1) {
                solveBlock(left, right);
                left = right + 1;
            }
        }

        int64 answer = 0;
        for (int i = 1; i <= n; ++i) answer += dp[i];
        cout << answer << '\n';
    }
}
