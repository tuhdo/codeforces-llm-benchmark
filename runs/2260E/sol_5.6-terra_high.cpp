#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    string s;
    cin >> n >> q >> s;

    vector<int> zeros(n + 1), changes(n + 1);
    for (int i = 0; i < n; ++i) {
        zeros[i + 1] = zeros[i] + (s[i] == '0');
        changes[i + 1] = changes[i] + (i > 0 && s[i] != s[i - 1]);
    }

    while (q--) {
        int l, r;
        cin >> l >> r;
        --l;
        --r;

        const int length = r - l + 1;
        const int zeroCount = zeros[r + 1] - zeros[l];
        const int oneCount = length - zeroCount;

        // Number of 01 (and also 10) edges in the substring viewed as a cycle.
        const int cyclicChanges = changes[r + 1] - changes[l] + (s[l] != s[r]);
        const int existingRunPairs = cyclicChanges / 2;

        int k = 0;
        k = max(k, (zeroCount + 1) / 2);
        k = max(k, (oneCount + 1) / 2);
        k = max(k, existingRunPairs);
        k = max(k, (length - existingRunPairs + 2) / 3);

        cout << 4 * k - length << '\n';
    }
}
