#include <bits/stdc++.h>
using namespace std;

class SegmentTree {
    int n;
    vector<int> minimum, lazy;

    void build(int v, int l, int r) {
        minimum[v] = l;
        if (l == r) return;
        int m = (l + r) / 2;
        build(v * 2, l, m);
        build(v * 2 + 1, m + 1, r);
    }

    void apply(int v, int value) {
        minimum[v] = max(minimum[v], value);
        lazy[v] = max(lazy[v], value);
    }

    void push(int v) {
        apply(v * 2, lazy[v]);
        apply(v * 2 + 1, lazy[v]);
        lazy[v] = 0;
    }

    // Check the old minimum, then impose the new lower bound.
    bool update(int v, int l, int r, int ql, int qr, int value) {
        if (qr < l || r < ql) return false;
        if (ql <= l && r <= qr) {
            bool possible = minimum[v] < value;
            apply(v, value);
            return possible;
        }
        push(v);
        int m = (l + r) / 2;
        bool left = update(v * 2, l, m, ql, qr, value);
        bool right = update(v * 2 + 1, m + 1, r, ql, qr, value);
        minimum[v] = min(minimum[v * 2], minimum[v * 2 + 1]);
        return left || right;
    }

public:
    explicit SegmentTree(int size) : n(size), minimum(4 * size), lazy(4 * size) {
        build(1, 1, n);
    }

    bool update(int l, int r, int value) {
        return l <= r && update(1, 1, n, l, r, value);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int LIMIT = 200001;
    vector<int> spf(LIMIT + 1);
    vector<int> primePowers;
    for (int p = 2; p <= LIMIT; ++p) {
        if (spf[p]) continue;
        for (int j = p; j <= LIMIT; j += p)
            if (!spf[j]) spf[j] = p;
        for (long long q = p; q <= LIMIT; q *= p)
            primePowers.push_back(static_cast<int>(q));
    }
    sort(primePowers.begin(), primePowers.end());

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<vector<int>> positions(n + 1);
        for (int i = 1; i <= n; ++i) {
            int x;
            cin >> x;
            while (x > 1) {
                int p = spf[x], power = 1;
                do {
                    x /= p;
                    power *= p;
                    positions[power].push_back(i);
                } while (x > 1 && spf[x] == p);
            }
        }

        SegmentTree tree(n);
        vector<int> answer;
        for (int q : primePowers) {
            bool possible = false;
            int previous = 0;
            if (q <= n) {
                for (int pos : positions[q]) {
                    possible |= tree.update(previous + 1, pos, pos);
                    previous = pos;
                }
            }
            possible |= tree.update(previous + 1, n, n + 1);
            if (possible) answer.push_back(q);
            if (previous == 0) break;
        }
        cout << answer.size() << '\n';
        for (int q : answer) cout << q << ' ';
        cout << '\n';
    }
}
