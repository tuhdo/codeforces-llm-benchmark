#include <bits/stdc++.h>
using namespace std;
int main() {
    int t;
    scanf("%d", &t);
    while (t--) {
        int n;
        scanf("%d", &n);
        vector<int> b(n), df(n + 2, 0);
        for (auto &x : b) scanf("%d", &x);
        for (int i = 0; i < n; i++)
            if (b[i] > 0) {
                int l = max(0, i - b[i] + 1), r = min(n - 1, i + b[i] - 1);
                df[l]++;
                df[r + 1]--;
            }
        string s(n, '0');
        int c = 0;
        for (int i = 0; i < n; i++) {
            c += df[i];
            if (!c) s[i] = '1';
        }
        const int INF = 1e9;
        vector<int> d(n, INF);
        int last = -INF;
        for (int i = 0; i < n; i++) {
            if (s[i] == '1') last = i;
            if (last >= 0) d[i] = i - last;
        }
        last = INF;
        for (int i = n - 1; i >= 0; i--) {
            if (s[i] == '1') last = i;
            if (last < INF) d[i] = min(d[i], last - i);
        }
        bool ok = s.find('1') != string::npos;
        for (int i = 0; i < n && ok; i++)
            if (b[i] >= 0 && d[i] != b[i]) ok = false;
        if (ok) puts(s.c_str());
        else puts("-1");
    }
}
