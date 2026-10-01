#include <bits/stdc++.h>
using namespace std;

struct SegTree {
    static constexpr int NEG = -1000000000;
    struct Node { int mn, smn, mx, lazy; };
    int n;
    vector<Node> st;

    SegTree(int n_) : n(n_), st(4 * n_ + 4, {NEG, INT_MAX, NEG, NEG}) {}

    void pull(int p) {
        const Node &a = st[p << 1], &b = st[p << 1 | 1];
        st[p].mx = max(a.mx, b.mx);
        st[p].mn = min(a.mn, b.mn);
        st[p].smn = min(a.mn == st[p].mn ? a.smn : a.mn,
                        b.mn == st[p].mn ? b.smn : b.mn);
    }

    void apply_chmax(int p, int x) {
        if (x <= st[p].mn) return;
        if (st[p].mn == st[p].mx) {
            st[p].mn = st[p].mx = x;
        } else {
            st[p].mn = x;
        }
        st[p].lazy = max(st[p].lazy, x);
    }

    void push(int p) {
        if (st[p].lazy != NEG) {
            apply_chmax(p << 1, st[p].lazy);
            apply_chmax(p << 1 | 1, st[p].lazy);
            st[p].lazy = NEG;
        }
    }

    void chmax(int p, int l, int r, int ql, int qr, int x) {
        if (qr < l || r < ql || x <= st[p].mn) return;
        if (ql <= l && r <= qr && x < st[p].smn) {
            apply_chmax(p, x);
            return;
        }
        if (l == r) { apply_chmax(p, x); return; }
        push(p);
        int m = (l + r) >> 1;
        chmax(p << 1, l, m, ql, qr, x);
        chmax(p << 1 | 1, m + 1, r, ql, qr, x);
        pull(p);
    }

    void chmax(int l, int r, int x) { if (l <= r) chmax(1, 1, n, l, r, x); }

    void setval(int p, int l, int r, int at, int x) {
        if (l == r) { st[p] = {x, INT_MAX, x, NEG}; return; }
        push(p);
        int m = (l + r) >> 1;
        if (at <= m) setval(p << 1, l, m, at, x);
        else setval(p << 1 | 1, m + 1, r, at, x);
        pull(p);
    }

    void setval(int at, int x) { setval(1, 1, n, at, x); }

    void add(int p, int l, int r, int at, int x) {
        if (l == r) {
            if (st[p].mx > NEG / 2) st[p] = {st[p].mx + x, INT_MAX, st[p].mx + x, NEG};
            return;
        }
        push(p);
        int m = (l + r) >> 1;
        if (at <= m) add(p << 1, l, m, at, x);
        else add(p << 1 | 1, m + 1, r, at, x);
        pull(p);
    }

    void add(int at, int x) { add(1, 1, n, at, x); }

    int get(int p, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return NEG;
        if (ql <= l && r <= qr) return st[p].mx;
        push(p);
        int m = (l + r) >> 1;
        return max(get(p << 1, l, m, ql, qr), get(p << 1 | 1, m + 1, r, ql, qr));
    }

    int get(int l, int r) { return l <= r ? get(1, 1, n, l, r) : NEG; }
    int get(int at) { return get(at, at); }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n, m;
        cin >> n >> m;
        vector<int> a(n + 1);
        for (int i = 1; i <= n; ++i) cin >> a[i];
        int starts = n - m + 1;
        SegTree dp(starts);

        // At the first cell, only the interval starting at 1 can cover it.
        int owner = 1 - a[1] + 1;
        dp.setval(1, owner == 1 ? 1 : 0);

        for (int i = 2; i <= n; ++i) {
            int left_prev = max(1, i - m);
            int right_prev = min(starts, i - 1);
            int enter_best = (i <= starts) ? dp.get(left_prev, right_prev) : SegTree::NEG;

            int expired_owner = i - m;
            int expire_best = SegTree::NEG;
            if (expired_owner >= 1 && expired_owner <= starts)
                expire_best = dp.get(expired_owner);

            if (i <= starts) dp.setval(i, enter_best);
            if (expired_owner >= 1 && expired_owner <= starts)
                dp.setval(expired_owner, SegTree::NEG);

            int left_now = max(1, i - m + 1);
            int right_now = min(starts, i);
            if (expire_best > SegTree::NEG / 2)
                dp.chmax(left_now, right_now, expire_best);

            int wanted = i - a[i] + 1;
            if (wanted >= left_now && wanted <= right_now)
                dp.add(wanted, 1);
        }

        cout << n - dp.get(starts) << '\n';
    }
    return 0;
}
