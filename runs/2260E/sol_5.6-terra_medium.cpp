#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    if (!(cin >> n >> q)) return 0;
    string s;
    cin >> s;

    // pref0[i] is the number of zeroes in s[0..i), and pref01/pref10
    // count directed adjacent transitions whose right endpoint is before i.
    vector<int> pref0(n + 1), pref01(n + 1), pref10(n + 1);
    for (int i = 0; i < n; ++i) {
        pref0[i + 1] = pref0[i] + (s[i] == '0');
        pref01[i + 1] = pref01[i];
        pref10[i + 1] = pref10[i];
        if (i > 0) {
            if (s[i - 1] == '0' && s[i] == '1') ++pref01[i + 1];
            if (s[i - 1] == '1' && s[i] == '0') ++pref10[i + 1];
        }
    }

    while (q--) {
        int l, r;
        cin >> l >> r;
        --l; // use a zero-based inclusive range
        int len = r - l;
        int z = pref0[r] - pref0[l];
        int o = len - z;
        int c01 = pref01[r] - pref01[l + 1];
        int c10 = pref10[r] - pref10[l + 1];

        int k = max({1, (z + 1) / 2, (o + 1) / 2, c01, c10});

        // If no insertion is available at this k, the substring itself has
        // to already be cyclically balanced.  Otherwise one more unit of
        // every edge type is sufficient.
        if (4 * k == len) {
            // Internal equal pairs follow from length and transition counts;
            // add the closing pair explicitly.
            int internal00 = z - c01 - (s[r - 1] == '0');
            int internal11 = o - c10 - (s[r - 1] == '1');
            int p00 = internal00 + (s[r - 1] == '0' && s[l] == '0');
            int p11 = internal11 + (s[r - 1] == '1' && s[l] == '1');
            int p01 = c01 + (s[r - 1] == '0' && s[l] == '1');
            int p10 = c10 + (s[r - 1] == '1' && s[l] == '0');
            if (!(p00 == k && p01 == k && p10 == k && p11 == k)) ++k;
        }

        cout << 4 * k - len << '\n';
    }
}
