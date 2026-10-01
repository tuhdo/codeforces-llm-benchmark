#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

// Operation (i,j) (0-indexed): swap rows i,i+1 inside columns j,j+1.
struct Solver {
    int m;
    vector<vector<int>> col;
    vector<int> ops; // i*256+j
    size_t abortLimit = (size_t)1 << 60;
    bool aborted = false;
    mt19937 rng;
    bool randomize = false;

    vector<int> dl, dpos, F, posF, cyc, dq;
    int bad = 0;

    Solver(int m_, const vector<vector<int>>& g) : m(m_), col(g) {
        dpos.assign(m, -1);
        F.assign(m, 0);
        posF.assign(m + 1, 0);
        cyc.assign(m, 0);
    }

    void op(int i, int j) {
        swap(col[j][i], col[j][i + 1]);
        swap(col[j + 1][i], col[j + 1][i + 1]);
        ops.push_back(i * 256 + j);
    }

    bool isSorted(int c) {
        for (int i = 0; i < m; i++)
            if (col[c][i] != i + 1) return false;
        return true;
    }

    void setd(int i, bool v) {
        if (i < 0 || i >= m - 1) return;
        bool cur = dpos[i] >= 0;
        if (v == cur) return;
        if (v) {
            dpos[i] = dl.size();
            dl.push_back(i);
        } else {
            int p = dpos[i];
            int last = dl.back();
            dl[p] = last;
            dpos[last] = p;
            dl.pop_back();
            dpos[i] = -1;
        }
    }

    void computeCyc() {
        fill(cyc.begin(), cyc.end(), -1);
        int id = 0;
        bad = 0;
        for (int p = 0; p < m; p++) {
            if (cyc[p] < 0) {
                int q = p, len = 0;
                while (cyc[q] < 0) {
                    cyc[q] = id;
                    q = F[q] - 1;
                    len++;
                }
                if (len > 1) bad += len;
                id++;
            }
        }
    }

    void pushD(int i) {
        if (i >= 0 && i <= m - 2) dq.push_back(i);
    }

    // Sort column a using ops that also move column a+dir; optionally fix column a+dir
    // using ops shared with column a+2*dir (value-swap corrections).
    void run(int a, int dir, bool allowCorr) {
        int B = a + dir, Cc = a + 2 * dir;
        int tAB = min(a, B);
        int tBC = allowCorr ? min(B, Cc) : 0;
        vector<int>& A = col[a];
        vector<int>& Bc = col[B];
        dl.clear();
        fill(dpos.begin(), dpos.end(), -1);
        for (int i = 0; i + 1 < m; i++)
            if (A[i] > A[i + 1]) setd(i, true);
        bool corr = allowCorr;
        if (corr) {
            for (int p = 0; p < m; p++) F[A[p] - 1] = Bc[p];
            for (int p = 0; p < m; p++) posF[F[p]] = p;
            computeCyc();
            if (bad == 0) corr = false;
            else {
                dq.clear();
                for (int i = 0; i + 1 < m; i++) dq.push_back(i);
            }
        }
        while (true) {
            if (ops.size() > abortLimit) {
                aborted = true;
                return;
            }
            if (corr) {
                while (!dq.empty()) {
                    int i = dq.back();
                    dq.pop_back();
                    int u = Bc[i], v = Bc[i + 1];
                    if (cyc[posF[u]] == cyc[posF[v]]) {
                        op(i, tBC);
                        int pu = posF[u], pv = posF[v];
                        F[pu] = v;
                        F[pv] = u;
                        posF[u] = pv;
                        posF[v] = pu;
                        computeCyc();
                        pushD(i - 1);
                        pushD(i);
                        pushD(i + 1);
                        if (bad == 0) {
                            corr = false;
                            dq.clear();
                            break;
                        }
                    }
                }
            }
            if (dl.empty()) break;
            int sz = dl.size();
            int ch = randomize ? dl[rng() % sz] : dl.back();
            if (corr) {
                int K = min(sz, 16);
                int off = randomize ? (int)(rng() % sz) : 0;
                for (int t = 0; t < K; t++) {
                    int idx = randomize ? (off + t) % sz : sz - 1 - t;
                    int i = dl[idx];
                    bool ok = false;
                    if (i - 1 >= 0) {
                        int u = Bc[i - 1], v = Bc[i + 1];
                        if (cyc[posF[u]] == cyc[posF[v]]) ok = true;
                    }
                    if (!ok && i + 2 < m) {
                        int u = Bc[i], v = Bc[i + 2];
                        if (cyc[posF[u]] == cyc[posF[v]]) ok = true;
                    }
                    if (ok) {
                        ch = i;
                        break;
                    }
                }
            }
            op(ch, tAB);
            setd(ch, false);
            setd(ch - 1, ch - 1 >= 0 && A[ch - 1] > A[ch]);
            setd(ch + 1, ch + 2 < m && A[ch + 1] > A[ch + 2]);
            if (corr) {
                pushD(ch - 1);
                pushD(ch);
                pushD(ch + 1);
            }
        }
    }

    void leftSweep(int z, bool corr) {
        for (int j = 0; j <= z - 1; j++) {
            if (isSorted(j)) continue;
            bool ac = corr && (j + 1 <= z - 1);
            run(j, +1, ac);
            if (aborted) return;
        }
    }

    void rightSweep(int z, bool corr) {
        for (int k = m - 1; k >= z + 1; k--) {
            if (isSorted(k)) continue;
            bool ac = corr && (k - 1 >= z + 1);
            run(k, -1, ac);
            if (aborted) return;
        }
    }

    // Applies an even permutation to column z alone, using commutators
    // across the op types z-1 and z.
    bool gadget(int z) {
        vector<int> a = col[z];
        vector<int> xs;
        for (int k = 1; k < m; k++) {
            int p = k;
            while (p > 0 && a[p - 1] > a[p]) {
                swap(a[p - 1], a[p]);
                xs.push_back(p - 1);
                p--;
            }
        }
        if (xs.size() % 2) return false;
        auto adj = [&](int x, int y) {
            op(y, z - 1);
            op(x, z);
            op(y, z - 1);
            op(x, z);
        };
        for (size_t t = 0; t < xs.size(); t += 2) {
            int x = xs[t], y = xs[t + 1];
            if (x == y) continue;
            if (abs(x - y) == 1) adj(x, y);
            else if (y > x) {
                for (int k = x; k < y; k++) adj(k, k + 1);
            } else {
                for (int k = x; k > y; k--) adj(k, k - 1);
            }
            if (ops.size() > abortLimit) {
                aborted = true;
                return false;
            }
        }
        return true;
    }

    bool solve(int z, bool corr) {
        leftSweep(z, corr);
        if (aborted) return false;
        rightSweep(z, corr);
        if (aborted) return false;
        for (int c = 0; c < m; c++)
            if (c != z && !isSorted(c)) return false;
        if (!isSorted(z)) {
            if (!gadget(z)) return false;
        }
        for (int c = 0; c < m; c++)
            if (!isSorted(c)) return false;
        return true;
    }
};

static int permParity(const vector<int>& a) {
    int m = a.size();
    vector<char> vis(m, 0);
    int cycles = 0;
    for (int i = 0; i < m; i++) {
        if (!vis[i]) {
            cycles++;
            int q = i;
            while (!vis[q]) {
                vis[q] = 1;
                q = a[q] - 1;
            }
        }
    }
    return (m - cycles) & 1;
}

// ---- exact BFS for n = 2 ----
static vector<array<int, 4>> P24;
static int idx24[256];
static int sw24[24][3];
static vector<signed char> bfsPar;
static bool bfsReady = false;

static void applyOp4(int c[4], int i, int j) {
    c[j] = sw24[c[j]][i];
    c[j + 1] = sw24[c[j + 1]][i];
}

static void prepBFS() {
    if (bfsReady) return;
    bfsReady = true;
    array<int, 4> p = {0, 1, 2, 3};
    do P24.push_back(p);
    while (next_permutation(p.begin(), p.end()));
    for (int i = 0; i < 24; i++) {
        auto& q = P24[i];
        idx24[q[0] * 64 + q[1] * 16 + q[2] * 4 + q[3]] = i;
    }
    for (int i = 0; i < 24; i++)
        for (int k = 0; k < 3; k++) {
            auto q = P24[i];
            swap(q[k], q[k + 1]);
            sw24[i][k] = idx24[q[0] * 64 + q[1] * 16 + q[2] * 4 + q[3]];
        }
    int N = 24 * 24 * 24 * 24;
    bfsPar.assign(N, -2);
    vector<int> q;
    q.reserve(N);
    q.push_back(0);
    bfsPar[0] = -1;
    for (size_t h = 0; h < q.size(); h++) {
        int s = q[h];
        int c[4] = {s / 13824 % 24, s / 576 % 24, s / 24 % 24, s % 24};
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++) {
                int e[4] = {c[0], c[1], c[2], c[3]};
                applyOp4(e, i, j);
                int t = ((e[0] * 24 + e[1]) * 24 + e[2]) * 24 + e[3];
                if (bfsPar[t] == -2) {
                    bfsPar[t] = i * 3 + j;
                    q.push_back(t);
                }
            }
    }
}

int main() {
    auto t0 = chrono::steady_clock::now();
    auto elapsed = [&]() {
        return chrono::duration<double>(chrono::steady_clock::now() - t0).count();
    };
    int T;
    scanf("%d", &T);
    vector<int> ns(T);
    vector<vector<vector<int>>> grids(T);
    ll sumCube = 0;
    for (int t = 0; t < T; t++) {
        int n;
        scanf("%d", &n);
        ns[t] = n;
        int m = 2 * n;
        vector<vector<int>> g(m, vector<int>(m)); // g[col][row]
        for (int r = 0; r < m; r++)
            for (int c = 0; c < m; c++) scanf("%d", &g[c][r]);
        grids[t] = g;
        sumCube += (ll)n * n * n;
    }
    string out;
    ll remCube = sumCube;
    const double TOTAL = 1.6;
    for (int t = 0; t < T; t++) {
        int n = ns[t], m = 2 * n;
        auto& g = grids[t];
        ll cube = (ll)n * n * n;
        double now = elapsed();
        double share = max(0.0, (TOTAL - now)) * (double)cube / (double)remCube;
        double testDeadline = now + share;
        remCube -= cube;
        ll budget = (ll)n * ((ll)n * (2 * n - 1)) + 9LL * n;

        // parity check
        int p0 = permParity(g[0]);
        bool okPar = true;
        for (int c = 1; c < m; c++)
            if (permParity(g[c]) != p0) okPar = false;
        if (!okPar) {
            out += "-1\n";
            continue;
        }
        if (n == 1) {
            if (g[0][0] == 1) out += "0\n";
            else out += "1\n1 1\n";
            continue;
        }
        if (n == 2) {
            prepBFS();
            int c[4];
            for (int k = 0; k < 4; k++) {
                int code = (g[k][0] - 1) * 64 + (g[k][1] - 1) * 16 + (g[k][2] - 1) * 4 + (g[k][3] - 1);
                c[k] = idx24[code];
            }
            int s = ((c[0] * 24 + c[1]) * 24 + c[2]) * 24 + c[3];
            if (bfsPar[s] == -2) {
                out += "-1\n";
                continue;
            }
            vector<pair<int, int>> path;
            while (s != 0) {
                int o = bfsPar[s];
                int i = o / 3, j = o % 3;
                path.push_back({i + 1, j + 1});
                int e[4] = {s / 13824 % 24, s / 576 % 24, s / 24 % 24, s % 24};
                applyOp4(e, i, j);
                s = ((e[0] * 24 + e[1]) * 24 + e[2]) * 24 + e[3];
            }
            if ((ll)path.size() > budget) {
                out += "-1\n";
                continue;
            }
            out += to_string(path.size()) + "\n";
            for (auto& pr : path) out += to_string(pr.first) + " " + to_string(pr.second) + "\n";
            continue;
        }

        vector<int> zs;
        {
            vector<int> cand = {m - 2, 1, m - 3, 2, m / 2, m / 2 - 1, m / 2 + 1, m / 2 - 2, m / 2 + 2};
            for (int z : cand) {
                if (z < 1 || z > m - 2) continue;
                if (find(zs.begin(), zs.end(), z) == zs.end()) zs.push_back(z);
            }
        }
        vector<int> best;
        bool have = false;
        bool done = false;
        int trials = 0;
        for (int rep = 0; rep < 40 && !done; rep++) {
            for (int z : zs) {
                for (int corr = 1; corr >= 0; corr--) {
                    if (trials > 0 && elapsed() > testDeadline) {
                        done = true;
                        break;
                    }
                    Solver s(m, g);
                    s.randomize = rep > 0;
                    s.rng.seed(rep * 7919 + z * 31 + corr);
                    s.abortLimit = have ? best.size() - 1 : (size_t)(8 * budget);
                    bool ok = s.solve(z, corr);
                    trials++;
                    if (ok && (!have || s.ops.size() < best.size())) {
                        best = s.ops;
                        have = true;
                    }
                    if (have && (ll)best.size() <= budget) {
                        done = true;
                        break;
                    }
                }
                if (done) break;
            }
        }
        if (!have) {
            out += "-1\n";
            continue;
        }
        out += to_string(best.size()) + "\n";
        for (int v : best) {
            out += to_string(v / 256 + 1);
            out += ' ';
            out += to_string(v % 256 + 1);
            out += '\n';
        }
    }
    fputs(out.c_str(), stdout);
    return 0;
}
