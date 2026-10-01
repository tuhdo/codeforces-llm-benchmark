#include <bits/stdc++.h>
using namespace std;

struct SegmentTree {
    struct Node {
        long long sum_c = 0;
        long long sum_s = 0;
        long long lazy_c = 0;
        long long lazy_p = 0;
        long long lazy_q = 0;
    };

    int n;
    vector<Node> tree;

    explicit SegmentTree(int n) : n(n), tree(4 * n) {}

    void apply(int p, int l, int r, long long c, long long coeff, long long q) {
        Node &x = tree[p];
        long long old_c_sum = x.sum_c;
        x.sum_c += c * (r - l + 1LL);
        x.sum_s += coeff * old_c_sum + q * (r - l + 1LL);

        x.lazy_q += coeff * x.lazy_c + q;
        x.lazy_c += c;
        x.lazy_p += coeff;
    }

    void push(int p, int l, int r) {
        if (l == r) return;
        Node &x = tree[p];
        if (x.lazy_c == 0 && x.lazy_p == 0 && x.lazy_q == 0) return;
        int m = (l + r) / 2;
        apply(p * 2, l, m, x.lazy_c, x.lazy_p, x.lazy_q);
        apply(p * 2 + 1, m + 1, r, x.lazy_c, x.lazy_p, x.lazy_q);
        x.lazy_c = x.lazy_p = x.lazy_q = 0;
    }

    void pull(int p) {
        tree[p].sum_c = tree[p * 2].sum_c + tree[p * 2 + 1].sum_c;
        tree[p].sum_s = tree[p * 2].sum_s + tree[p * 2 + 1].sum_s;
    }

    void add(int p, int l, int r, int ql, int qr, long long delta) {
        if (ql > r || qr < l) return;
        if (ql <= l && r <= qr) {
            apply(p, l, r, delta, 0, 0);
            return;
        }
        push(p, l, r);
        int m = (l + r) / 2;
        add(p * 2, l, m, ql, qr, delta);
        add(p * 2 + 1, m + 1, r, ql, qr, delta);
        pull(p);
    }

    void add(int l, int r, long long delta) {
        if (l <= r) add(1, 0, n - 1, l, r, delta);
    }

    void multiply_c_into_s(long long delta) {
        apply(1, 0, n - 1, 0, delta, 0);
    }

    void collect(int p, int l, int r, vector<long long> &out) {
        if (l == r) {
            out[l] = tree[p].sum_s;
            return;
        }
        push(p, l, r);
        int m = (l + r) / 2;
        collect(p * 2, l, m, out);
        collect(p * 2 + 1, m + 1, r, out);
    }

    vector<long long> values() {
        vector<long long> result(n);
        collect(1, 0, n - 1, result);
        return result;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<long long> h(n);
        for (long long &x : h) cin >> x;

        vector<pair<long long, int>> order;
        order.reserve(n);
        for (int i = 0; i < n; ++i) order.push_back({h[i], i});
        sort(order.begin(), order.end(), greater<>());

        SegmentTree seg(n);
        set<int> active;

        auto distance_cyclic = [n](int from, int to) {
            int d = to - from;
            if (d < 0) d += n;
            return d;
        };

        auto add_forward = [&](int start, int length, long long delta) {
            if (length <= 0) return;
            int finish = start + length - 1;
            if (finish < n) seg.add(start, finish, delta);
            else {
                seg.add(start, n - 1, delta);
                seg.add(0, finish % n, delta);
            }
        };

        auto insert_active = [&](int x) {
            if (active.empty()) {
                active.insert(x);
                return;
            }

            auto it = active.lower_bound(x);
            int b = (it == active.end() ? *active.begin() : *it);
            int a = (it == active.begin() ? *active.rbegin() : *prev(it));
            int d = (a == b ? n : distance_cyclic(a, b));
            int r = distance_cyclic(a, x);
            int q = d - r;

            // Split the old inactive gap a -> b at x.
            add_forward((a + 1) % n, r - 1, q);
            seg.add(x, x, q);

            if (b == a) {
                add_forward((x + 1) % n, q - 1, r);
                seg.add(a, a, r);
            } else {
                add_forward((x + 1) % n, q - 1, r);
                seg.add(b, b, r);
            }
            active.insert(x);
        };

        int pos = 0;
        while (pos < n) {
            int end = pos;
            while (end < n && order[end].first == order[pos].first) {
                insert_active(order[end].second);
                ++end;
            }
            long long next_height = (end == n ? 0 : order[end].first);
            seg.multiply_c_into_s(order[pos].first - next_height);
            pos = end;
        }

        vector<long long> answer = seg.values();
        for (int i = 0; i < n; ++i) {
            if (i) cout << ' ';
            cout << answer[i];
        }
        cout << '\n';
    }
    return 0;
}
