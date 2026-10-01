#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int test_count;
    cin >> test_count;
    while (test_count--) {
        int n;
        cin >> n;

        vector<int> a(n), b(n);
        for (int& x : a) {
            cin >> x;
            --x;
        }
        for (int& x : b) {
            cin >> x;
            if (x != -1) --x;
        }

        bool possible = true;
        vector<bool> processed(n, false);

        // One known value determines the whole cycle containing its index.
        for (int start = 0; start < n && possible; ++start) {
            if (b[start] == -1 || processed[start]) continue;

            int v = start;
            while (!processed[v]) {
                processed[v] = true;
                int next_index = a[v];
                int required_value = a[b[v]];
                if (b[next_index] == -1) {
                    b[next_index] = required_value;
                } else if (b[next_index] != required_value) {
                    possible = false;
                    break;
                }
                v = next_index;
            }
        }

        vector<int> used(n, 0);
        for (int x : b) {
            if (x != -1) ++used[x];
        }
        for (int x : used) {
            if (x > 1) possible = false;
        }

        if (!possible) {
            cout << "NO\n";
            continue;
        }

        vector<vector<int>> empty_source(n + 1), empty_target(n + 1);
        vector<bool> seen_source(n, false), seen_target(n, false);

        // Each recorded start is the smallest element of its cycle, since the
        // outer loop visits vertices in increasing order.
        for (int start = 0; start < n; ++start) {
            if (b[start] != -1 || seen_source[start]) continue;
            int length = 0;
            for (int v = start; !seen_source[v]; v = a[v]) {
                seen_source[v] = true;
                ++length;
            }
            empty_source[length].push_back(start);
        }
        for (int start = 0; start < n; ++start) {
            if (used[start] != 0 || seen_target[start]) continue;
            int length = 0;
            for (int v = start; !seen_target[v]; v = a[v]) {
                seen_target[v] = true;
                ++length;
            }
            empty_target[length].push_back(start);
        }

        for (int length = 1; length <= n; ++length) {
            if (empty_source[length].size() != empty_target[length].size()) {
                possible = false;
                break;
            }
            for (int k = 0; k < (int)empty_source[length].size(); ++k) {
                int source = empty_source[length][k];
                int target = empty_target[length][k];
                do {
                    b[source] = target;
                    source = a[source];
                    target = a[target];
                } while (source != empty_source[length][k]);
            }
        }

        if (!possible) {
            cout << "NO\n";
            continue;
        }

        cout << "YES\n";
        for (int i = 0; i < n; ++i) {
            cout << b[i] + 1 << " \n"[i + 1 == n];
        }
    }
}
