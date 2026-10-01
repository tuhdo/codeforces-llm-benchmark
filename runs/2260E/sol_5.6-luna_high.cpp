#include <bits/stdc++.h>
using namespace std;

static bool possible(int k, int base0, int base1, int zeros, int ones) {
    int need = max(base0, base1);
    need += max(0, zeros - k - base0);
    need += max(0, ones - k - base1);
    return need <= k;
}

static int minimum_k(int base0, int base1, int zeros, int ones, int upper_bound) {
    int lo = 1, hi = upper_bound;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (possible(mid, base0, base1, zeros, ones)) {
            hi = mid;
        } else {
            lo = mid + 1;
        }
    }
    return lo;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;
    string s;
    cin >> s;

    vector<int> prefix_zeros(n + 1), prefix_changes(n + 1);
    for (int i = 1; i <= n; ++i) {
        prefix_zeros[i] = prefix_zeros[i - 1] + (s[i - 1] == '0');
        if (i >= 2) {
            prefix_changes[i] = prefix_changes[i - 1] + (s[i - 1] != s[i - 2]);
        }
    }

    while (q--) {
        int l, r;
        cin >> l >> r;

        int length = r - l + 1;
        int zeros = prefix_zeros[r] - prefix_zeros[l - 1];
        int ones = length - zeros;

        int changes = prefix_changes[r] - prefix_changes[l];
        int runs = changes + 1;
        int zero_runs;
        if (s[l - 1] == '0') {
            zero_runs = (runs + 1) / 2;
        } else {
            zero_runs = runs / 2;
        }
        int one_runs = runs - zero_runs;

        int best = minimum_k(zero_runs, one_runs, zeros, ones, length);

        if (s[l - 1] == s[r - 1] && zero_runs > 0 && one_runs > 0) {
            int shared_endpoint_base = min(zero_runs, one_runs);
            best = min(best, minimum_k(shared_endpoint_base,
                                       shared_endpoint_base,
                                       zeros, ones, length));
        }

        cout << 4 * best - length << '\n';
    }
    return 0;
}
