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
        vector<int> p(n + 1), parent(n + 1), firstChild(n + 1, n + 1);
        for (int i = 1; i <= n; ++i) cin >> p[i];

        vector<int> st;
        st.reserve(n);
        for (int i = n; i >= 1; --i) {
            while (!st.empty() && p[st.back()] < p[i]) st.pop_back();
            if (!st.empty()) {
                parent[i] = st.back();
                firstChild[parent[i]] = i;
            }
            st.push_back(i);
        }

        vector<int> root(n + 1), marked(n + 1), highest(n + 1);
        vector<long long> direct(n + 1);
        long long answer = 1LL * n * (n - 1) / 2;

        for (int i = n; i >= 1; --i) {
            int a = parent[i];
            if (a == 0) {
                root[i] = i;
                continue;
            }

            root[i] = root[a];
            // Sum for paths consisting of right moves and one optional final left move.
            direct[i] = 2LL * (a - i) - 1 + root[i] - a + direct[a];

            bool hasEarlierChild = firstChild[a] < i;
            marked[i] = marked[a] + hasEarlierChild;
            highest[i] = highest[a] ? highest[a] : (hasEarlierChild ? a : 0);

            int m = highest[i];
            if (m == 0) {
                answer += direct[i];
                continue;
            }

            // All destinations through m take at most three moves.
            long long middle = 3LL * (m - i);
            middle -= a - i - 1;  // Right, then left: distance two.
            middle -= 2;          // The next-greater position: distance one.
            middle -= marked[a]; // Left, then right to an ancestor above a.

            int b = parent[a];
            if (b != 0 && b <= m && firstChild[b] == a) {
                --middle; // Two right moves, not already counted above.
            }

            int stepsToM = (m == a ? 1 : 2);
            answer += middle + direct[m] + 1LL * stepsToM * (root[i] - m);
        }

        cout << answer << '\n';
    }
    return 0;
}
