#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;
    string s;
    cin >> s;

    vector<int> ones(n + 1), changes(n + 1);
    for (int i = 0; i < n; ++i) {
        ones[i + 1] = ones[i] + (s[i] == '1');
        changes[i + 1] = changes[i] + (i > 0 && s[i] != s[i - 1]);
    }

    while (q--) {
        int l, r;
        cin >> l >> r;
        const int length = r - l + 1;
        const int one_count = ones[r] - ones[l - 1];
        const int zero_count = length - one_count;
        const int cyclic_changes = changes[r] - changes[l]
                                 + (s[l - 1] != s[r - 1]);
        const int k = cyclic_changes / 2;
        const int x = max({k, (zero_count + 1) / 2,
                           (one_count + 1) / 2, (length - k + 2) / 3});
        cout << 4 * x - length << '\n';
    }
}
