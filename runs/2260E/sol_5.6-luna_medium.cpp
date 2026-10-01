#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;
    string s;
    cin >> s;
    s = " " + s;

    vector<int> zero(n + 1), one(n + 1), down(n + 1), up(n + 1);
    for (int i = 1; i <= n; ++i) {
        zero[i] = zero[i - 1] + (s[i] == '0');
        one[i] = one[i - 1] + (s[i] == '1');
        down[i] = down[i - 1];
        up[i] = up[i - 1];
        if (i > 1) {
            down[i] += (s[i - 1] == '1' && s[i] == '0');
            up[i] += (s[i - 1] == '0' && s[i] == '1');
        }
    }

    while (q--) {
        int l, r;
        cin >> l >> r;

        int zeros = zero[r] - zero[l - 1];
        int ones = one[r] - one[l - 1];
        int zero_runs = (s[l] == '0') + down[r] - down[l];
        int one_runs = (s[l] == '1') + up[r] - up[l];
        if (s[l] == s[r]) {
            if (s[l] == '0') --zero_runs;
            else --one_runs;
        }

        int k = max({(zeros + 1) / 2, (ones + 1) / 2, zero_runs, one_runs});
        cout << 4 * k - (r - l + 1) << '\n';
    }
}
