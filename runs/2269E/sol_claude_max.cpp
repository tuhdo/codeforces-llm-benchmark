#include <bits/stdc++.h>
using namespace std;

// Energy of a segment = (a_l ^ ... ^ a_r) & max = (P[r] ^ P[l-1]) & m.
// F(s) = "some segment (l<r) has s as a submask of both its XOR and its max".
// F is downward closed, so the answer is built greedily from the top bit.
// To test F(s): with K[y] = P[y] & s, we need a node j (a_j ⊇ s, designated max
// of the segment) and u in [pl_j, j-1], v in [j, nr_j-1], (u,v) != (j-1,j),
// K[u] ^ K[v] = s. Iterate the smaller side of each node, and answer
// "next/previous occurrence of a key" in O(1) with a sweep.

static const int LOG = 18;
static const int SZ = 1 << LOG;
static const int INF = 1000000000;

static int nxtOcc[SZ], prvOcc[SZ];

int main() {
    int t;
    if (scanf("%d", &t) != 1) return 0;
    fill(nxtOcc, nxtOcc + SZ, INF);
    fill(prvOcc, prvOcc + SZ, -1);
    while (t--) {
        int n;
        scanf("%d", &n);
        vector<int> a(n + 2, 0), P(n + 1, 0), pl(n + 2, 0), nr(n + 2, 0), K(n + 1, 0);
        vector<char> leftIter(n + 2, 0);
        for (int i = 1; i <= n; i++) {
            scanf("%d", &a[i]);
            P[i] = P[i - 1] ^ a[i];
        }
        {
            vector<int> st;
            for (int j = 1; j <= n; j++) {
                while (!st.empty() && a[st.back()] < a[j]) st.pop_back();
                pl[j] = st.empty() ? 0 : st.back();
                st.push_back(j);
            }
            st.clear();
            for (int j = n; j >= 1; j--) {
                while (!st.empty() && a[st.back()] <= a[j]) st.pop_back();
                nr[j] = st.empty() ? n + 1 : st.back();
                st.push_back(j);
            }
            for (int j = 1; j <= n; j++) leftIter[j] = (j - pl[j]) <= (nr[j] - j);
        }

        auto check = [&](int s) -> bool {
            for (int y = 0; y <= n; y++) K[y] = P[y] & s;
            bool found = false;

            // Smaller side is the left one: iterate u, look up v via next occurrence.
            for (int j = n; j >= 1; j--) {
                int ns = nxtOcc[K[j]];
                nxtOcc[K[j]] = j;
                if (found) continue;
                if (!leftIter[j] || (a[j] & s) != s) continue;
                int hi = nr[j] - 1;
                if (ns <= hi) { found = true; continue; }
                for (int u = pl[j]; u <= j - 2; u++) {
                    if (nxtOcc[K[u] ^ s] <= hi) { found = true; break; }
                }
            }
            for (int y = 1; y <= n; y++) nxtOcc[K[y]] = INF;

            // Smaller side is the right one: iterate v, look up u via previous occurrence.
            if (!found) {
                for (int j = 1; j <= n; j++) {
                    int ps = prvOcc[K[j - 1]];
                    prvOcc[K[j - 1]] = j - 1;
                    if (found) continue;
                    if (leftIter[j] || (a[j] & s) != s) continue;
                    int lo = pl[j];
                    if (ps >= lo) { found = true; continue; }
                    int hi = nr[j] - 1;
                    for (int v = j + 1; v <= hi; v++) {
                        if (prvOcc[K[v] ^ s] >= lo) { found = true; break; }
                    }
                }
                for (int y = 0; y < n; y++) prvOcc[K[y]] = -1;
            }
            return found;
        };

        int s = 0;
        for (int b = LOG - 1; b >= 0; b--) {
            int cand = s | (1 << b);
            if (check(cand)) s = cand;
        }
        printf("%d\n", s);
    }
    return 0;
}
