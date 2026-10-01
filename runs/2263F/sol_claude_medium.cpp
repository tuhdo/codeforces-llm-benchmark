#include <bits/stdc++.h>
using namespace std;

// Bessie's walk: nodes whose road must change ("hubs") can be jumped from freely until
// they are finalised (road raised to b); finalised/fixed nodes only follow b upwards.
// Start in the unique source SCC, then repeatedly finalise the smallest-index reachable hub.
static bool solve(int n, const vector<int>& a0, const vector<int>& b, int& start, vector<char>& ops) {
    vector<int> a = a0;
    ops.clear();
    start = 0;
    for (int i = 0; i < n; i++)
        if (a[i] > b[i]) return false;
    vector<int> hubId(n, -1), hubs;
    for (int i = 0; i < n; i++)
        if (a[i] < b[i]) { hubId[i] = (int)hubs.size(); hubs.push_back(i); }
    int H = (int)hubs.size();
    if (H == 0) return true;
    int X = hubs.back();

    // first hub on the chain i, b[i], b[b[i]], ... (or -1 if the chain ends in a fixed root)
    vector<int> res0(n);
    for (int u = n - 1; u >= 0; u--) {
        if (hubId[u] >= 0) res0[u] = u;
        else if (b[u] == u) res0[u] = -1;
        else res0[u] = res0[b[u]];
    }

    // static graph on hubs: jump edges and "finalise" edges
    vector<vector<int>> adj(H);
    {
        vector<int> mark(H, -1);
        for (int h = 0; h < H; h++) {
            int p = hubs[h];
            for (int u = a[p]; u < b[p]; u++) {
                int r = res0[u];
                if (r >= 0 && hubId[r] != h && mark[hubId[r]] != h) { mark[hubId[r]] = h; adj[h].push_back(hubId[r]); }
            }
            if (b[p] != p) {
                int r = res0[b[p]];
                if (r >= 0 && hubId[r] != h && mark[hubId[r]] != h) { mark[hubId[r]] = h; adj[h].push_back(hubId[r]); }
            }
        }
    }
    // Tarjan SCC (iterative)
    vector<int> idx(H, -1), low(H, 0), comp(H, -1), stk;
    vector<char> onst(H, 0);
    int timer = 0, nc = 0;
    {
        vector<pair<int, int>> cs;
        for (int s = 0; s < H; s++) {
            if (idx[s] >= 0) continue;
            cs.push_back({s, 0});
            idx[s] = low[s] = timer++;
            stk.push_back(s); onst[s] = 1;
            while (!cs.empty()) {
                int v = cs.back().first;
                int& ei = cs.back().second;
                if (ei < (int)adj[v].size()) {
                    int w = adj[v][ei++];
                    if (idx[w] < 0) {
                        idx[w] = low[w] = timer++;
                        stk.push_back(w); onst[w] = 1;
                        cs.push_back({w, 0});
                    } else if (onst[w]) low[v] = min(low[v], idx[w]);
                } else {
                    if (low[v] == idx[v]) {
                        while (true) {
                            int w = stk.back(); stk.pop_back(); onst[w] = 0;
                            comp[w] = nc;
                            if (w == v) break;
                        }
                        nc++;
                    }
                    cs.pop_back();
                    if (!cs.empty()) { int par = cs.back().first; low[par] = min(low[par], low[v]); }
                }
            }
        }
    }
    vector<char> hasIn(nc, 0);
    for (int h = 0; h < H; h++)
        for (int w : adj[h])
            if (comp[w] != comp[h]) hasIn[comp[w]] = 1;
    int srcComp = -1, srcCount = 0;
    for (int c = 0; c < nc; c++)
        if (!hasIn[c]) { srcCount++; srcComp = c; }
    if (srcCount != 1) return false;
    int s0 = -1;
    for (int h = 0; h < H; h++)
        if (comp[h] == srcComp) { s0 = hubs[h]; break; }
    start = s0;

    // simulation
    vector<char> alive(n, 0);
    for (int v : hubs) alive[v] = 1;
    int cnt = H;
    vector<int> dsu(n);
    for (int i = 0; i < n; i++) dsu[i] = (alive[i] || b[i] == i) ? i : b[i];
    auto findr = [&](int v) {
        int r = v;
        while (dsu[r] != r) r = dsu[r];
        while (dsu[v] != r) { int nx = dsu[v]; dsu[v] = r; v = nx; }
        return r;
    };
    auto res = [&](int u) { int r = findr(u); return alive[r] ? r : -1; };

    vector<int> seen(n, -1), par(n), parU(n), nxt(n + 1);
    auto nfind = [&](int v) {
        int r = v;
        while (nxt[r] != r) r = nxt[r];
        while (nxt[v] != r) { int t = nxt[v]; nxt[v] = r; v = t; }
        return r;
    };
    int p = s0;
    int stamp = 0;
    auto follow = [&]() { ops.push_back(2); p = a[p]; };
    auto raise = [&](int t) { while (a[p] < t) { a[p]++; ops.push_back(1); } };
    vector<int> qu;
    while (true) {
        stamp++;
        for (int i = 0; i <= n; i++) nxt[i] = i;
        qu.clear();
        qu.push_back(p);
        seen[p] = stamp; par[p] = -1;
        for (size_t h = 0; h < qu.size(); h++) {
            int x = qu[h];
            for (int u = nfind(a[x]); u < b[x]; u = nfind(u + 1)) {
                nxt[u] = u + 1;
                int r = res(u);
                if (r >= 0 && seen[r] != stamp) { seen[r] = stamp; par[r] = x; parU[r] = u; qu.push_back(r); }
            }
        }
        int q = -1;
        if (cnt == 1) {
            if (seen[X] != stamp) return false;
            q = X;
        } else {
            for (int v : hubs)
                if (alive[v] && v != X && seen[v] == stamp) { q = v; break; }
            if (q < 0) return false;
        }
        vector<pair<int, int>> path;
        for (int v = q; par[v] != -1; v = par[v]) path.push_back({par[v], parU[v]});
        reverse(path.begin(), path.end());
        for (auto& pr : path) {
            raise(pr.second);
            follow();
            while (!alive[p]) follow();
        }
        raise(b[q]);
        alive[q] = 0;
        if (b[q] != q) dsu[q] = b[q];
        cnt--;
        if (cnt == 0) return true;
        if (b[q] == q) return false;
        int r = findr(b[q]);
        if (!alive[r]) return false;
        follow();
        while (!alive[p]) follow();
    }
}

#ifndef NOMAIN
int main() {
    int t;
    if (scanf("%d", &t) != 1) return 0;
    string out;
    while (t--) {
        int n;
        scanf("%d", &n);
        vector<int> a(n), b(n);
        for (auto& x : a) { scanf("%d", &x); x--; }
        for (auto& x : b) { scanf("%d", &x); x--; }
        int start;
        vector<char> ops;
        if (!solve(n, a, b, start, ops)) { out += "-1\n"; continue; }
        out += to_string(ops.size()) + " " + to_string(start + 1) + "\n";
        for (size_t i = 0; i < ops.size(); i++) { out += char('0' + ops[i]); out += ' '; }
        out += "\n";
    }
    fputs(out.c_str(), stdout);
}
#endif
