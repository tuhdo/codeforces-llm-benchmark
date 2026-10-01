#include <bits/stdc++.h>
using namespace std;
typedef __int128 LL;
typedef unsigned __int128 ULL;

// class: 0 atom, 1 product-level, 2 sum-level
struct E { string s; int c; bool ok = false; };
const int LIM = 400;
E best[LIM + 1];

string par(const E& e, int need) { return e.c <= need ? e.s : "(" + e.s + ")"; }

bool consider(int v, const string& s, int c) {
    if (v <= 0 || v > LIM) return false;
    if (!best[v].ok || s.size() < best[v].s.size()) {
        best[v] = {s, c, true};
        return true;
    }
    return false;
}

void buildConsts() {
    consider(1, "n/n", 1);
    bool changed = true;
    while (changed) {
        changed = false;
        for (int a = 1; a <= LIM; a++) {
            if (!best[a].ok) continue;
            for (int b = 1; b <= LIM; b++) {
                if (!best[b].ok) continue;
                E A = best[a], B = best[b];
                if (consider(a + b, par(A, 2) + "+" + par(B, 1), 2)) changed = true;
                if (consider(a * b, par(A, 1) + "*" + par(B, 1), 1)) changed = true;
                if (a > b && consider(a - b, par(A, 2) + "-" + par(B, 1), 2)) changed = true;
                if (a % b == 0 && consider(a / b, par(A, 1) + "/" + par(B, 0), 1)) changed = true;
            }
        }
    }
}

string big(ULL N) {
    string bestS;
    for (int base = 2; base < 200; base++) {
        if (!best[base].ok) continue;
        vector<int> digs;
        ULL t = N;
        while (t) { digs.push_back((int)(t % base)); t /= base; }
        reverse(digs.begin(), digs.end());
        string bp = par(best[base], 1);
        string s;
        bool first = true;
        bool good = true;
        for (int d : digs) {
            if (first) {
                if (!best[d].ok) { good = false; break; }
                s = par(best[d], 2);
                first = false;
            } else {
                s = "(" + s + ")*" + bp;
                if (d) {
                    if (!best[d].ok) { good = false; break; }
                    s += "+" + par(best[d], 1);
                }
            }
        }
        if (good && (bestS.empty() || s.size() < bestS.size())) bestS = s;
    }
    return bestS;
}

string step(int m) {
    // factor is m when n > m, else 1; z = 2(n-m)-1 is odd, I = round(1/2 + 1/(2z)) = [z > 0]
    int c = m - 1, k = 2 * m + 1;
    string I = "round(n/(n+n)+n/(n+n)/(n+n-" + par(best[k], 1) + "))";
    return "(n/n+" + par(best[c], 1) + "*" + I + ")";
}

// convergents of e
bool findConv(ULL& p, ULL& q) {
    vector<int> cf = {2};
    for (int i = 1; i < 80; i++) cf.push_back(i % 3 == 2 ? 2 * ((i + 1) / 3) : 1);
    ULL h0 = 1, h1 = 2, k0 = 0, k1 = 1;
    for (size_t i = 1; i < cf.size(); i++) {
        ULL h2 = cf[i] * h1 + h0, k2 = cf[i] * k1 + k0;
        if (h2 >> 120) return false;
        h0 = h1; h1 = h2; k0 = k1; k1 = k2;
        // check: n! * q / p rounds to D(n) for all n in 2..50
        ULL P = h1, Q = k1;
        ULL f = 1, D = 1;  // D(0)=1
        bool ok = true;
        // iterate n, wrapping mod 2^128
        ULL Dm = 0;  // D(1)
        f = 1;
        for (int n = 2; n <= 50 && ok; n++) {
            f *= n;
            Dm = Dm * n + ((n % 2 == 0) ? 1 : (ULL)-1);
            LL R = (LL)(f * Q - Dm * P);
            if (R < 0) R = -R;
            if (!(R * 2 < (LL)P)) ok = false;
        }
        if (ok) { p = h1; q = k1; return true; }
    }
    return false;
}

#ifdef TEST
struct Fr { LL n, d; };
LL gcdl(LL a, LL b) { if (a < 0) a = -a; if (b < 0) b = -b; while (b) { LL t = a % b; a = b; b = t; } return a; }
Fr mk(LL n, LL d) { if (d == 0) { fprintf(stderr, "div0\n"); exit(1); } if (d < 0) { n = -n; d = -d; } LL g = gcdl(n, d); if (g == 0) g = 1; return {n / g, d / g}; }
string S; size_t pos; LL NV;
Fr expr();
Fr atom() {
    if (S[pos] == 'n') { pos++; return {NV, 1}; }
    if (S[pos] == '(') { pos++; Fr r = expr(); pos++; return r; }
    pos += 6;  // round(
    Fr r = expr(); pos++;
    // floor(r + 1/2)
    LL num = 2 * r.n + r.d, den = 2 * r.d;
    LL fl = num / den; if (num % den != 0 && num < 0) fl--;
    return {fl, 1};
}
Fr term() {
    Fr a = atom();
    while (pos < S.size() && (S[pos] == '*' || S[pos] == '/')) {
        char o = S[pos++]; Fr b = atom();
        a = o == '*' ? mk(a.n * b.n, a.d * b.d) : mk(a.n * b.d, a.d * b.n);
    }
    return a;
}
Fr expr() {
    Fr a = term();
    while (pos < S.size() && (S[pos] == '+' || S[pos] == '-')) {
        char o = S[pos++]; Fr b = term();
        a = o == '+' ? mk(a.n * b.d + b.n * a.d, a.d * b.d) : mk(a.n * b.d - b.n * a.d, a.d * b.d);
    }
    return a;
}
Fr ev(const string& s, int n) { S = s; pos = 0; NV = n; return expr(); }
#endif

int main() {
    buildConsts();
    ULL p, q;
    if (!findConv(p, q)) return 1;
    string ps = big(p), qs = big(q);
    string prod = "n";
    for (int m = 2; m <= 49; m++) prod += "*" + step(m);
    string expr = "round(" + prod + "*(" + qs + ")/(" + ps + "))";
#ifdef TEST
    fprintf(stderr, "len %zu\n", expr.size());
    for (int n = 1; n <= 50; n++) {
        for (int m = 2; m <= 49; m++) {
            Fr r = ev(step(m), n);
            if (r.d != 1 || r.n != (n > m ? m : 1)) { fprintf(stderr, "bad step %d %d\n", m, n); return 1; }
        }
        Fr a = ev(qs, n), b = ev(ps, n);
        if (a.d != 1 || b.d != 1 || (ULL)a.n != q || (ULL)b.n != p) { fprintf(stderr, "bad const\n"); return 1; }
    }
    fprintf(stderr, "verified\n");
#endif
    puts(expr.c_str());
}
