#include <bits/stdc++.h>
using namespace std;

struct Node {
    long long len = 0;
    int xr = 0;
    long long pref[2] = {0, 0}; // Prefixes, including the empty prefix.
    long long suff[2] = {0, 0}; // Suffixes, including the empty suffix.
    long long odd = 0;
};

static Node merge_nodes(const Node& a, const Node& b) {
    if (a.len == 0) return b;
    if (b.len == 0) return a;
    Node c;
    c.len = a.len + b.len;
    c.xr = a.xr ^ b.xr;
    for (int p = 0; p < 2; ++p) {
        c.pref[p] = a.pref[p] + b.pref[p ^ a.xr] - (p == a.xr);
        c.suff[p] = b.suff[p] + a.suff[p ^ b.xr] - (p == b.xr);
    }
    c.odd = a.odd + b.odd;
    long long as[2] = {a.suff[0], a.suff[1]};
    long long bp[2] = {b.pref[0], b.pref[1]};
    --as[0]; // Exclude the empty suffix of the left segment.
    --bp[0]; // Exclude the empty prefix of the right segment.
    c.odd += as[0] * bp[1] + as[1] * bp[0];
    return c;
}

struct SegTree {
    int n;
    vector<Node> tree;
    SegTree(const vector<int>& a) : n((int)a.size()), tree(4 * max(1, (int)a.size())) {
        if (n) build(1, 0, n - 1, a);
    }
    void build(int v, int l, int r, const vector<int>& a) {
        if (l == r) {
            Node& x = tree[v];
            x.len = 1; x.xr = a[l];
            x.pref[0] = x.suff[0] = 1;
            x.pref[a[l]]++; x.suff[a[l]]++;
            x.odd = a[l];
            return;
        }
        int m = (l + r) / 2;
        build(v * 2, l, m, a); build(v * 2 + 1, m + 1, r, a);
        tree[v] = merge_nodes(tree[v * 2], tree[v * 2 + 1]);
    }
    void toggle(int pos) { if (n) toggle(1, 0, n - 1, pos); }
    void toggle(int v, int l, int r, int p) {
        if (l == r) {
            Node& x = tree[v];
            x.xr ^= 1;
            x.pref[0] = x.suff[0] = x.pref[1] = x.suff[1] = 0;
            x.pref[0] = x.suff[0] = 1;
            x.pref[x.xr]++; x.suff[x.xr]++;
            x.odd = x.xr;
            return;
        }
        int m = (l + r) / 2;
        if (p <= m) toggle(v * 2, l, m, p); else toggle(v * 2 + 1, m + 1, r, p);
        tree[v] = merge_nodes(tree[v * 2], tree[v * 2 + 1]);
    }
    const Node& root() const { return tree[1]; }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T; cin >> T;
    while (T--) {
        int n, q; cin >> n >> q;
        string s; cin >> s;
        vector<int> edges(max(0, n - 1));
        for (int i = 0; i + 1 < n; ++i) edges[i] = (s[i] != s[i + 1]);
        SegTree st(edges);
        long long totalOnes = 0;
        for (int i = 0; i + 1 < n; ++i)
            totalOnes += (edges[i] ? 1LL : 0LL) * (i + 1LL) * (n - 1 - i);
        auto answer = [&]() -> long long {
            if (n <= 1) return 0;
            const Node& x = st.root();
            return (totalOnes + x.odd) / 2;
        };
        cout << answer();
        while (q--) {
            int i; cin >> i; --i;
            s[i] = (s[i] == '0' ? '1' : '0');
            if (i > 0) {
                int e = i - 1;
                totalOnes += edges[e] ? -(e + 1LL) * (n - 1 - e) : (e + 1LL) * (n - 1 - e);
                edges[e] ^= 1; st.toggle(e);
            }
            if (i + 1 < n) {
                int e = i;
                totalOnes += edges[e] ? -(e + 1LL) * (n - 1 - e) : (e + 1LL) * (n - 1 - e);
                edges[e] ^= 1; st.toggle(e);
            }
            cout << ' ' << answer();
        }
        cout << '\n';
    }
}
