#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

const int MAXN = 1'000'005;

int n;
int p[MAXN], nxtPos[MAXN], firstPred[MAXN];
int64 dp[MAXN];
bool marked[MAXN];
vector<int> events[MAXN];

void solveBlock(int left, int right) {
    dp[right] = right - left;

    int rightmost = right;
    int markedCount = 0;

    // Initially, positions j with firstPred[j] <= right are marked.
    // Only positions inside this block can matter.
    if (firstPred[right] != n + 1) {
        events[firstPred[right]].push_back(right);
        marked[right] = true;
        ++markedCount;
    }

    for (int i = right - 1; i >= left; --i) {
        while (rightmost >= left && !marked[rightmost]) --rightmost;

        if (rightmost == nxtPos[i]) {
            dp[i] = dp[nxtPos[i]] + (right - left + 1) - 2 - (i - left);
        } else {
            // Number of positions at distance two from i.
            int c2 = nxtPos[i] - i - 1;
            if (nxtPos[nxtPos[i]] != n + 1) ++c2;
            c2 += markedCount - 1;

            // nxt(nxt(i)) may already be among the marked positions.
            if (nxtPos[nxtPos[i]] != n + 1 &&
                firstPred[nxtPos[nxtPos[i]]] <= i) {
                --c2;
            }

            dp[i] = dp[rightmost] + 2LL * (right - left + 1) - 3 -
                    2LL * (i - left + 1) - c2 + 1;
        }

        for (int j : events[i]) {
            marked[j] = false;
            --markedCount;
        }
        if (firstPred[i] != n + 1) {
            events[firstPred[i]].push_back(i);
            marked[i] = true;
            ++markedCount;
        }
    }

    // Positions to the left of the block are all reachable in one move.
    for (int i = left; i <= right; ++i) dp[i] += left - 1;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        cin >> n;

        for (int i = 1; i <= n; ++i) {
            cin >> p[i];
            events[i].clear();
            marked[i] = false;
        }

        fill(firstPred + 1, firstPred + n + 1, n + 1);

        // Compute nearest greater elements to the right using path jumping.
        for (int i = n; i >= 1; --i) {
            nxtPos[i] = i + 1;
            while (nxtPos[i] <= n && p[nxtPos[i]] < p[i]) {
                nxtPos[i] = nxtPos[nxtPos[i]];
            }
            firstPred[nxtPos[i]] = i;
        }

        int left = 1;
        for (int right = 1; right <= n; ++right) {
            if (nxtPos[right] == n + 1) {
                solveBlock(left, right);
                left = right + 1;
            }
        }

        int64 answer = 0;
        for (int i = 1; i <= n; ++i) answer += dp[i];
        cout << answer << '\n';
    }
}
