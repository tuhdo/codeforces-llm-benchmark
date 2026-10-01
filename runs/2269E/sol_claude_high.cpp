#include <bits/stdc++.h>
using namespace std;

static const int MAXN = 200005;
static const int B = 18;
int lc[(MAXN + 2) * (B + 1)], rc_[(MAXN + 2) * (B + 1)], cnt[(MAXN + 2) * (B + 1)];
int tot;
int roots[MAXN + 2];

int ins(int prev, int val) {
    int root = ++tot, cur = root;
    int p = prev;
    cnt[cur] = cnt[p] + 1;
    for (int b = B - 1; b >= 0; b--) {
        int bit = (val >> b) & 1;
        int nn = ++tot;
        int pc = bit ? rc_[p] : lc[p];
        int po = bit ? lc[p] : rc_[p];
        if (bit) { rc_[cur] = nn; lc[cur] = po; }
        else { lc[cur] = nn; rc_[cur] = po; }
        cnt[nn] = cnt[pc] + 1;
        cur = nn;
        p = pc;
    }
    lc[cur] = rc_[cur] = 0;
    return root;
}

int best, qx, qm;

void dfs(int u, int v, int b, int cur) {
    // bits b..0 remain; cur holds chosen bits above b
    int rem = b >= 0 ? (qm & ((2 << b) - 1)) : 0;
    if (cur + rem <= best) return;
    if (rem == 0) { best = cur; return; }
    int xb = (qx >> b) & 1;
    int mb = (qm >> b) & 1;
    int c0u = lc[u], c0v = lc[v], c1u = rc_[u], c1v = rc_[v];
    auto go = [&](int t, int nv) {
        int uu = t ? c1u : c0u, vv = t ? c1v : c0v;
        if (cnt[uu] - cnt[vv] > 0) dfs(uu, vv, b - 1, nv);
    };
    if (mb) {
        go(xb ^ 1, cur | (1 << b));
        go(xb, cur);
    } else {
        go(0, cur);
        go(1, cur);
    }
}

void query(int x, int A, int Bi, int m) {
    if (A > Bi) return;
    int hi = roots[Bi + 1], lo = roots[A];
    qx = x & m;
    qm = m;
    dfs(hi, lo, B - 1, 0);
}

int main() {
    int t;
    scanf("%d", &t);
    while (t--) {
        int n;
        scanf("%d", &n);
        vector<int> a(n + 2), S(n + 1, 0);
        for (int i = 1; i <= n; i++) {
            scanf("%d", &a[i]);
            S[i] = S[i - 1] ^ a[i];
        }
        tot = 0;
        cnt[0] = lc[0] = rc_[0] = 0;
        roots[0] = 0;
        for (int i = 0; i <= n; i++) roots[i + 1] = ins(roots[i], S[i]);
        vector<int> Lb(n + 2), Rb(n + 2);
        {
            vector<int> st;
            for (int i = 1; i <= n; i++) {
                while (!st.empty() && a[st.back()] < a[i]) st.pop_back();
                Lb[i] = st.empty() ? 1 : st.back() + 1;
                st.push_back(i);
            }
            st.clear();
            for (int i = n; i >= 1; i--) {
                while (!st.empty() && a[st.back()] <= a[i]) st.pop_back();
                Rb[i] = st.empty() ? n : st.back() - 1;
                st.push_back(i);
            }
        }
        vector<int> ord(n);
        iota(ord.begin(), ord.end(), 1);
        sort(ord.begin(), ord.end(), [&](int x, int y) { return a[x] > a[y]; });
        best = 0;
        for (int p : ord) {
            int m = a[p];
            if (m <= best) break;
            int L = Lb[p], R = Rb[p];
            if (L == R) continue;
            if (p - L + 1 <= R - p + 1) {
                for (int l = L; l <= p; l++)
                    query(S[l - 1], max(p, l + 1), R, m);
            } else {
                for (int r = p; r <= R; r++)
                    query(S[r], L - 1, min(p, r - 1) - 1, m);
            }
        }
        printf("%d\n", best);
    }
    return 0;
}
