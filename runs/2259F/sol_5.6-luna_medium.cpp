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

        vector<int> a(n);
        for (int &v : a) cin >> v;
        string s;
        cin >> s;

        vector<int> q;
        int zeros = 0;
        for (int i = n - 1; i >= 0; --i) {
            if (a[i] == 0) {
                ++zeros;
            } else {
                q.push_back(zeros);
            }
        }
        reverse(q.begin(), q.end());

        int ones = (int)q.size();
        vector<long long> pref(ones + 1, 0);
        for (int i = 0; i < ones; ++i) {
            pref[i + 1] = pref[i] + q[i];
        }

        auto current_inversions = [&](int bubble_count, int reverse_count) -> long long {
            int left = bubble_count;
            if (left >= ones) return 0;

            // q is non-increasing. Find the number of rows with q_i > reverse_count.
            int lo = left, hi = ones;
            while (lo < hi) {
                int mid = (lo + hi) / 2;
                if (q[mid] > reverse_count) lo = mid + 1;
                else hi = mid;
            }
            int right = lo;
            if (right <= left) return 0;
            return (pref[right] - pref[left]) - 1LL * reverse_count * (right - left);
        };

        int bubble_count = 0;
        int reverse_count = 0;
        for (int step = 0; step <= n; ++step) {
            if (step) {
                if (s[step - 1] == '1') ++bubble_count;
                else ++reverse_count;
            }
            if (step) cout << ' ';
            cout << current_inversions(bubble_count, reverse_count);
        }
        cout << '\n';
    }
    return 0;
}
