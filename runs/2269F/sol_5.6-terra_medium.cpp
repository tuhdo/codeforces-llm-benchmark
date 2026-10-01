#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        vector<int> p(n);
        for (int& x : p) cin >> x;

        vector<int> st;
        st.reserve(n);
        vector<long long> subtreeSize(n), forwardSum(n), totalSum(n);

        for (int i = 0; i < n; ++i) {
            vector<int> children; // Popped from right to left.
            while (!st.empty() && p[st.back()] < p[i]) {
                children.push_back(st.back());
                st.pop_back();
            }

            subtreeSize[i] = 1;
            long long childrenSize = 0;
            long long childrenTotal = 0;
            for (int child : children) {
                childrenSize += subtreeSize[child];
                childrenTotal += totalSum[child];
            }

            if (!children.empty()) {
                // The last popped child is the leftmost child. It is the only
                // child visited before a direct child of i becomes available.
                int first = children.back();
                forwardSum[i] = forwardSum[first] + subtreeSize[first];
                subtreeSize[i] += subtreeSize[first];

                for (int k = 0; k + 1 < (int)children.size(); ++k) {
                    int child = children[k];
                    forwardSum[i] += 2 * subtreeSize[child] - 1;
                    subtreeSize[i] += subtreeSize[child];
                }
            }

            // In this ordered list, the root itself is the final block.
            // A later block reaches every earlier block in one left move.
            long long prefixSize = 0;
            long long reverseCross = 0;
            for (int k = (int)children.size() - 1; k >= 0; --k) {
                int child = children[k];
                reverseCross += subtreeSize[child] * prefixSize;
                prefixSize += subtreeSize[child];
            }
            reverseCross += prefixSize;

            // An earlier child reaches a later child by first reaching i,
            // then moving left once.  The root itself needs no final move.
            long long forwardCross = 0;
            long long usedSize = 0;
            for (int k = (int)children.size() - 1; k >= 0; --k) {
                int child = children[k];
                long long laterSize = childrenSize - usedSize - subtreeSize[child];
                long long toRoot;
                if (k == (int)children.size() - 1) {
                    toRoot = forwardSum[child] + subtreeSize[child];
                } else {
                    toRoot = 2 * subtreeSize[child] - 1;
                }
                forwardCross += toRoot * (laterSize + 1) + subtreeSize[child] * laterSize;
                usedSize += subtreeSize[child];
            }
            totalSum[i] = childrenTotal + reverseCross + forwardCross;
            st.push_back(i);
        }

        long long answer = 0;
        long long prefixSize = 0;
        for (int root : st) {
            answer += totalSum[root] + subtreeSize[root] * prefixSize;
            prefixSize += subtreeSize[root];
        }
        cout << answer << '\n';
    }
    return 0;
}
