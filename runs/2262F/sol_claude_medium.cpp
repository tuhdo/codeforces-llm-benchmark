#include <bits/stdc++.h>
using namespace std;
typedef bitset<300> bs;
typedef pair<int,int> pii;

struct St { bs A[300], AT[300], B[300], BT[300]; };
static St cur, sv, tmp;
static int n;
static mt19937 rng(987654321);

static void upd(St& s, const bs& x0, const bs& y0) {
    bs x = x0, y = y0;
    for (int j = x._Find_first(); j < 300; j = x._Find_next(j)) s.B[j] ^= y;
    for (int i = y._Find_first(); i < 300; i = y._Find_next(i)) s.BT[i] ^= x;
}
static void rem(St& s, int i, int j) { s.A[i][j] = 0; s.AT[j][i] = 0; }
static void single(St& s, int i, int j) { upd(s, s.BT[i], s.B[j]); rem(s, i, j); }
static void colPair(St& s, int j, int i1, int i2) {
    upd(s, s.BT[i1] ^ s.BT[i2], s.B[j]); rem(s, i1, j); rem(s, i2, j);
}
static void rowPair(St& s, int i, int j1, int j2) {
    upd(s, s.BT[i], s.B[j1] ^ s.B[j2]); rem(s, i, j1); rem(s, i, j2);
}

static bool findZero(St& s, int& zi, int& zj) {
    int o = rng() % n;
    for (int k = 0; k < n; k++) {
        int i = (o + k) % n;
        bs z = s.A[i] & ~s.BT[i];
        if (z.any()) { zi = i; zj = z._Find_first(); return true; }
    }
    return false;
}
static int findGroup(St& s) {
    int o = rng() % (2 * n);
    for (int k = 0; k < 2 * n; k++) {
        int g = (o + k) % (2 * n);
        int c = g < n ? (int)s.A[g].count() : (int)s.AT[g - n].count();
        if (c >= 3) return g;
    }
    return -1;
}
static void pickTwo(const bs& Z, const bs& O, int& a, int& b) {
    const bs& S = (Z.count() >= 2) ? Z : O;
    vector<int> v;
    for (int j = S._Find_first(); j < 300; j = S._Find_next(j)) v.push_back(j);
    int p = rng() % v.size();
    swap(v[0], v[p]);
    int q = 1 + rng() % (v.size() - 1);
    a = v[0]; b = v[q];
}
static void pairFromGroup(St& s, int g, vector<pii>& out) {
    int a, b;
    if (g < n) {
        int i = g;
        bs Z = s.A[i] & ~s.BT[i], O = s.A[i] & s.BT[i];
        pickTwo(Z, O, a, b);
        rowPair(s, i, a, b);
        out.push_back({i, a}); out.push_back({i, b});
    } else {
        int j = g - n;
        bs Z = s.AT[j] & ~s.B[j], O = s.AT[j] & s.B[j];
        pickTwo(Z, O, a, b);
        colPair(s, j, a, b);
        out.push_back({a, j}); out.push_back({b, j});
    }
}
static bool stuckPair(St& s, vector<pii>& out) {
    for (int att = 0; att < 80; att++) {
        int g = findGroup(s);
        if (g < 0) return false;
        tmp = s;
        vector<pii> o2;
        pairFromGroup(tmp, g, o2);
        int zi, zj;
        if (findZero(tmp, zi, zj)) {
            s = tmp;
            for (auto& p : o2) out.push_back(p);
            return true;
        }
    }
    return false;
}
static bool runBatch(St& s, vector<pii>& out) {
    int N = n;
    while (N > 0) {
        int zi, zj;
        bool hz = findZero(s, zi, zj);
        if (N & 1) {
            if (hz) { single(s, zi, zj); out.push_back({zi, zj}); N--; }
            else if (N >= 3) { if (!stuckPair(s, out)) return false; N -= 2; }
            else return false;
        } else {
            int g = findGroup(s);
            if (g >= 0) { pairFromGroup(s, g, out); N -= 2; }
            else {
                if (!hz) return false;
                single(s, zi, zj); out.push_back({zi, zj}); N--;
            }
        }
    }
    return true;
}
static int rankOf(St& s) {
    static bs r[300];
    for (int i = 0; i < n; i++) r[i] = s.A[i];
    int rk = 0;
    for (int c = 0; c < n; c++) {
        int p = -1;
        for (int i = rk; i < n; i++) if (r[i][c]) { p = i; break; }
        if (p < 0) continue;
        swap(r[p], r[rk]);
        for (int i = 0; i < n; i++) if (i != rk && r[i][c]) r[i] ^= r[rk];
        rk++;
    }
    return rk;
}
static int matchR[300];
static bool vis[300];
static bool kuhn(int i) {
    for (int j = cur.A[i]._Find_first(); j < 300; j = cur.A[i]._Find_next(j)) {
        if (vis[j]) continue;
        vis[j] = true;
        if (matchR[j] < 0 || kuhn(matchR[j])) { matchR[j] = i; return true; }
    }
    return false;
}

int main() {
    int t;
    scanf("%d", &t);
    string outS;
    while (t--) {
        int m;
        scanf("%d %d", &n, &m);
        for (int i = 0; i < n; i++) { cur.A[i].reset(); cur.AT[i].reset(); }
        for (int k = 0; k < m; k++) {
            int x, y; scanf("%d %d", &x, &y); x--; y--;
            cur.A[x][y] = 1; cur.AT[y][x] = 1;
        }
        // inverse
        {
            static bs a[300], c[300];
            for (int i = 0; i < n; i++) { a[i] = cur.A[i]; c[i].reset(); c[i][i] = 1; }
            for (int col = 0; col < n; col++) {
                int p = -1;
                for (int i = col; i < n; i++) if (a[i][col]) { p = i; break; }
                if (p < 0) continue;
                swap(a[p], a[col]); swap(c[p], c[col]);
                for (int i = 0; i < n; i++) if (i != col && a[i][col]) { a[i] ^= a[col]; c[i] ^= c[col]; }
            }
            for (int i = 0; i < n; i++) { cur.B[i] = c[i]; cur.BT[i].reset(); }
            for (int j = 0; j < n; j++)
                for (int i = 0; i < n; i++) if (cur.B[j][i]) cur.BT[i][j] = 1;
        }
        vector<vector<pii>> mv;
        int mm = m;
        bool fb = false;
        while (mm > 2 * n) {
            sv = cur;
            vector<pii> out;
            bool ok = false;
            for (int att = 0; att < 40 && !ok; att++) {
                cur = sv; out.clear();
                ok = runBatch(cur, out);
            }
            if (!ok) { cur = sv; fb = true; break; }
            mv.push_back(out);
            mm -= n;
        }
        if (fb) {
            while (true) {
                vector<pii> ones;
                for (int i = 0; i < n; i++)
                    for (int j = cur.A[i]._Find_first(); j < 300; j = cur.A[i]._Find_next(j)) ones.push_back({i, j});
                if (ones.empty()) break;
                int r = rankOf(cur);
                vector<pii> out(ones.begin(), ones.begin() + r);
                for (auto& p : out) cur.A[p.first][p.second] = 0;
                mv.push_back(out);
            }
        } else if (mm == n) {
            vector<pii> out;
            for (int i = 0; i < n; i++)
                for (int j = cur.A[i]._Find_first(); j < 300; j = cur.A[i]._Find_next(j)) out.push_back({i, j});
            mv.push_back(out);
        } else {
            for (int j = 0; j < n; j++) matchR[j] = -1;
            for (int i = 0; i < n; i++) { memset(vis, 0, sizeof vis); kuhn(i); }
            vector<pii> P;
            for (int j = 0; j < n; j++) if (matchR[j] >= 0) P.push_back({matchR[j], j});
            int k2 = mm - n;
            vector<pii> S2(P.begin(), P.begin() + k2);
            for (auto& p : S2) cur.A[p.first][p.second] = 0;
            vector<pii> S1;
            for (int i = 0; i < n; i++)
                for (int j = cur.A[i]._Find_first(); j < 300; j = cur.A[i]._Find_next(j)) S1.push_back({i, j});
            mv.push_back(S1);
            mv.push_back(S2);
        }
        outS += to_string(mv.size()) + "\n";
        for (auto& mvv : mv) {
            outS += to_string(mvv.size()) + "\n";
            for (auto& p : mvv) {
                outS += to_string(p.first + 1); outS += ' '; outS += to_string(p.second + 1); outS += '\n';
            }
        }
    }
    fputs(outS.c_str(), stdout);
}
