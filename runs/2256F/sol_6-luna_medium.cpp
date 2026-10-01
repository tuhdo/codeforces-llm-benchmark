#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        int max_bits = 0;
        for (int i = 0; i < n; ++i) {
            unsigned int a;
            cin >> a;
            max_bits = max(max_bits, 32 - __builtin_clz(a));
        }
        cout << max(n, max_bits) << '\n';
    }
    return 0;
}