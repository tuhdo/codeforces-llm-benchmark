#include <bits/stdc++.h>
using namespace std;

// A persistent binary trie over prefix xors.  A query may ignore bits which
// are zero in mask; those branches are both explored only when necessary.
struct PersistentTrie {
    struct Node { int ch[2] = {}, cnt = 0; };
    vector<Node> tr;
    vector<int> root;
    vector<int> pref;

    PersistentTrie(const vector<int>& p) : pref(p) {
        tr.reserve((int)p.size() * 20 + 1);
        tr.push_back({});
        root.resize(p.size() + 1);
        for (int i = 0; i < (int)p.size(); ++i) root[i + 1] = add(root[i], 17, p[i]);
    }

    int add(int old, int bit, int value) {
        int cur = (int)tr.size(); tr.push_back(tr[old]);
        ++tr[cur].cnt;
        if (bit < 0) return cur;
        int b = (value >> bit) & 1;
        tr[cur].ch[b] = add(tr[old].ch[b], bit - 1, value);
        return cur;
    }

    int amount(int hi, int lo, int b, bool eraseOne, int erased) const {
        int ret = tr[tr[hi].ch[b]].cnt - tr[tr[lo].ch[b]].cnt;
        if (eraseOne && ((erased >> 17) & 1) == b) --ret;
        return ret;
    }

    int solve(int hi, int lo, int bit, int x, int mask, bool eraseOne, int erased) const {
        int total = tr[hi].cnt - tr[lo].cnt - (eraseOne ? 1 : 0);
        if (total <= 0 || bit < 0) return 0;
        int xb = (x >> bit) & 1;
        auto descend = [&](int b) {
            int nhi = tr[hi].ch[b], nlo = tr[lo].ch[b];
            bool nextErase = eraseOne && (((erased >> bit) & 1) == b);
            return solve(nhi, nlo, bit - 1, x, mask, nextErase, erased);
        };
        if ((mask >> bit) & 1) {
            int preferred = xb ^ 1;
            int have = tr[tr[hi].ch[preferred]].cnt - tr[tr[lo].ch[preferred]].cnt;
            if (eraseOne && ((erased >> bit) & 1) == preferred) --have;
            if (have) return (1 << bit) | descend(preferred);
            return descend(xb);
        }
        int best = 0;
        for (int b = 0; b < 2; ++b) {
            int have = tr[tr[hi].ch[b]].cnt - tr[tr[lo].ch[b]].cnt;
            if (eraseOne && ((erased >> bit) & 1) == b) --have;
            if (have) best = max(best, descend(b));
        }
        return best;
    }

    // Values stored at prefix positions [left, right], inclusive.
    int query(int left, int right, int x, int mask, int forbidden = -1) const {
        bool eraseOne = forbidden != -1;
        int erased = eraseOne ? pref[forbidden] : 0;
        return solve(root[right + 1], root[left], 17, x, mask, eraseOne, erased);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    if (!(cin >> T)) return 0;
    while (T--) {
        int n; cin >> n;
        vector<int> a(n), px(n + 1);
        for (int i = 0; i < n; ++i) { cin >> a[i]; px[i + 1] = px[i] ^ a[i]; }

        // A segment belongs to its leftmost maximum: no >= maximum on its
        // left, and no > maximum on its right.
        vector<int> prevGE(n), nextG(n);
        vector<int> st;
        for (int i = 0; i < n; ++i) {
            while (!st.empty() && a[st.back()] < a[i]) st.pop_back();
            prevGE[i] = st.empty() ? -1 : st.back();
            st.push_back(i);
        }
        st.clear();
        for (int i = n - 1; i >= 0; --i) {
            while (!st.empty() && a[st.back()] <= a[i]) st.pop_back();
            nextG[i] = st.empty() ? n : st.back();
            st.push_back(i);
        }

        PersistentTrie trie(px);
        vector<int> order(n);
        iota(order.begin(), order.end(), 0);
        sort(order.begin(), order.end(), [&](int i, int j) { return a[i] > a[j]; });

        int answer = 0;
        for (int i : order) {
            // No segment with this maximum can improve the answer.
            if (a[i] <= answer) break;
            int leftLo = prevGE[i] + 1, leftHi = i;  // prefix positions l
            int rightLo = i + 1, rightHi = nextG[i]; // prefix positions r+1
            int leftCount = leftHi - leftLo + 1;
            int rightCount = rightHi - rightLo + 1;
            if (leftCount <= rightCount) {
                for (int p = leftLo; p <= leftHi; ++p) {
                    // (i, i+1) is the one-element segment [i,i].
                    int banned = (p == i ? i + 1 : -1);
                    answer = max(answer, trie.query(rightLo, rightHi, px[p], a[i], banned));
                }
            } else {
                for (int p = rightLo; p <= rightHi; ++p) {
                    int banned = (p == i + 1 ? i : -1);
                    answer = max(answer, trie.query(leftLo, leftHi, px[p], a[i], banned));
                }
            }
        }
        cout << answer << '\n';
    }
}
