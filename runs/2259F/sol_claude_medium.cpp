#include <bits/stdc++.h>
using namespace std;
static char s[500005];
int main() {
    int t;
    scanf("%d", &t);
    while (t--) {
        int n;
        scanf("%d", &n);
        vector<long long> c;
        int o = 0;
        for (int i = 0; i < n; i++) {
            int x;
            scanf("%d", &x);
            if (x) o++;
            else c.push_back(o);
        }
        scanf("%s", s);
        int lo = 0, hi = c.size();
        long long S = 0, off = 0;
        for (auto v : c) S += v;
        while (lo < hi && c[lo] <= off) { S -= c[lo]; lo++; }
        string out = to_string(S - off * (hi - lo));
        for (int i = 0; i < n; i++) {
            if (s[i] == '1') {
                off++;
                while (lo < hi && c[lo] <= off) { S -= c[lo]; lo++; }
            } else if (hi > lo) {
                hi--;
                S -= c[hi];
            }
            out += ' ';
            out += to_string(S - off * (hi - lo));
        }
        puts(out.c_str());
    }
}
