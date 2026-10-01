#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;
    string s;
    cin >> s;

    vector<int> prefixOnes(n + 1, 0);
    vector<int> prefixChanges(n, 0);
    for (int i = 0; i < n; ++i) {
        prefixOnes[i + 1] = prefixOnes[i] + (s[i] == '1');
        if (i + 1 < n) {
            prefixChanges[i + 1] = prefixChanges[i] + (s[i] != s[i + 1]);
        }
    }

    while (q--) {
        int l, r;
        cin >> l >> r;
        --l;
        --r;
        int length = r - l + 1;
        if (length == 1) {
            cout << 3 << '\n';
            continue;
        }

        int ones = prefixOnes[r + 1] - prefixOnes[l];
        int zeros = length - ones;
        int changes = prefixChanges[r] - prefixChanges[l];
        if (s[l] != s[r]) ++changes;

        int count00 = zeros - changes / 2;
        int count11 = ones - changes / 2;

        int low = (length + 3) / 4;
        int high = length;
        while (low < high) {
            int k = low + (high - low) / 2;
            int breaksNeeded = max(0, count00 - k) + max(0, count11 - k);
            if (breaksNeeded + changes / 2 <= k) {
                high = k;
            } else {
                low = k + 1;
            }
        }

        cout << 4 * low - length << '\n';
    }
    return 0;
}