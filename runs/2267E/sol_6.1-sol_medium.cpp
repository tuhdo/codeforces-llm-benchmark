#include <iostream>
#include <string>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n, q;
        string s;
        cin >> n >> q >> s;

        long long zeros = 0;
        long long boundary_sum = 0;
        for (int i = 0; i < n; ++i) {
            zeros += (s[i] == '0');
            if (i + 1 < n && s[i] != s[i + 1]) {
                boundary_sum += 1LL * (i + 1) * (n - i - 1);
            }
        }

        auto answer = [&]() {
            return (boundary_sum + zeros * (n - zeros)) / 2;
        };
        cout << answer();

        while (q--) {
            int p;
            cin >> p;
            --p;

            for (int j = p - 1; j <= p; ++j) {
                if (j < 0 || j + 1 >= n) continue;
                long long weight = 1LL * (j + 1) * (n - j - 1);
                boundary_sum += (s[j] == s[j + 1] ? weight : -weight);
            }

            zeros += (s[p] == '0' ? -1 : 1);
            s[p] = (s[p] == '0' ? '1' : '0');
            cout << ' ' << answer();
        }
        cout << '\n';
    }
}
