#include <bits/stdc++.h>
using namespace std;

struct Node {
    long long pref[2] = {0, 0}; // parity of non-empty prefixes, plus empty prefix
    long long suff[2] = {0, 0}; // parity of non-empty suffixes, plus empty suffix
    long long odd = 0;          // odd-sum subarrays inside this segment
    int total = 0;              // xor of all values in this segment
};

Node merge_nodes(const Node& a, const Node& b) {
    Node r;
    for (int p = 0; p < 2; ++p) {
        r.pref[p] = a.pref[p] + b.pref[p ^ a.total] - (p == a.total);
        r.suff[p] = b.suff[p] + a.suff[p ^ b.total] - (p == b.total);
    }

    // Remove the empty prefix/suffix before counting subarrays crossing the cut.
    long long a_suf[2] = {a.suff[0] - 1, a.suff[1]};
    long long b_pre[2] = {b.pref[0] - 1, b.pref[1]};
    r.odd = a.odd + b.odd + a_suf[0] * b_pre[1] + a_suf[1] * b_pre[0];
    r.total = a.total ^ b.total;
    return r;
}

struct SegTree {
    int n;
    vector<Node> tree;

    explicit SegTree(const vector<int>& a) {
        n = (int)a.size();
        tree.assign(4 * max(1, n), Node{});
        if (n) build(1, 0, n - 1, a);
    }

    Node leaf(int value) {
        Node x;
        x.pref[value]++;
        x.suff[value]++;
        x.pref[0]++; // empty prefix
        x.suff[0]++; // empty suffix
        x.odd = value;
        x.total = value;
        return x;
    }

    void build(int v, int l, int r, const vector<int>& a) {
        if (l == r) {
            tree[v] = leaf(a[l]);
            return;
        }
        int mid = (l + r) / 2;
        build(v * 2, l, mid, a);
        build(v * 2 + 1, mid + 1, r, a);
        tree[v] = merge_nodes(tree[v * 2], tree[v * 2 + 1]);
    }

    void update(int pos, int value) { update(1, 0, n - 1, pos, value); }
    void update(int v, int l, int r, int pos, int value) {
        if (l == r) {
            tree[v] = leaf(value);
            return;
        }
        int mid = (l + r) / 2;
        if (pos <= mid) update(v * 2, l, mid, pos, value);
        else update(v * 2 + 1, mid + 1, r, pos, value);
        tree[v] = merge_nodes(tree[v * 2], tree[v * 2 + 1]);
    }

    long long odd_subarrays() const { return n ? tree[1].odd : 0; }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n, q;
        string s;
        cin >> n >> q >> s;
        int m = n - 1;
        vector<int> changes(max(0, m));
        long long weighted = 0;
        for (int j = 0; j < m; ++j) {
            changes[j] = (s[j] != s[j + 1]);
            if (changes[j]) weighted += 1LL * (j + 1) * (m - j);
        }
        SegTree seg(changes);

        auto answer = [&]() -> long long {
            return (weighted + seg.odd_subarrays()) / 2;
        };

        cout << answer();
        while (q--) {
            int pos;
            cin >> pos;
            --pos;
            s[pos] = (s[pos] == '0' ? '1' : '0');
            for (int j : {pos - 1, pos}) {
                if (j < 0 || j >= m) continue;
                int next_value = (s[j] != s[j + 1]);
                if (next_value != changes[j]) {
                    int delta = next_value - changes[j];
                    weighted += 1LL * delta * (j + 1) * (m - j);
                    changes[j] = next_value;
                    seg.update(j, next_value);
                }
            }
            cout << ' ' << answer();
        }
        cout << '\n';
    }
}
