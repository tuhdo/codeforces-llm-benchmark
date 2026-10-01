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
        vector<int> b(n), diff(n + 1, 0);
        for (int i = 0; i < n; ++i) {
            cin >> b[i];
            if (b[i] > 0) {
                int left = max(0, i - b[i] + 1);
                int right = min(n - 1, i + b[i] - 1);
                ++diff[left];
                --diff[right + 1];
            }
        }

        string answer(n, '0');
        int coverage = 0;
        bool has_treasure = false;
        for (int i = 0; i < n; ++i) {
            coverage += diff[i];
            if (coverage == 0) {
                answer[i] = '1';
                has_treasure = true;
            }
        }

        vector<int> distance(n, n);
        int nearest = -n;
        for (int i = 0; i < n; ++i) {
            if (answer[i] == '1') nearest = i;
            distance[i] = i - nearest;
        }
        nearest = 2 * n;
        bool valid = has_treasure;
        for (int i = n - 1; i >= 0; --i) {
            if (answer[i] == '1') nearest = i;
            distance[i] = min(distance[i], nearest - i);
            if (b[i] != -1 && b[i] != distance[i]) valid = false;
        }

        cout << (valid ? answer : "-1") << '\n';
    }
}
