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

        vector<long long> h(n);
        int peak = 0;
        for (int i = 0; i < n; ++i) {
            cin >> h[i];
            if (h[i] > h[peak]) peak = i;
        }

        // Rotate so the chosen maximum edge is the final element.
        vector<long long> a(n);
        for (int i = 0; i < n; ++i)
            a[i] = h[(peak + 1 + i) % n];

        // right[i]: sum of prefix maxima of a[i..n-2].
        vector<long long> right(n, 0);
        vector<int> st;
        for (int i = n - 2; i >= 0; --i) {
            while (!st.empty() && a[st.back()] < a[i]) st.pop_back();
            int j = st.empty() ? n - 1 : st.back();
            right[i] = a[i] * (j - i) + right[j];
            st.push_back(i);
        }

        // left[i]: sum of prefix maxima of a[i-1], a[i-2], ..., a[0].
        vector<long long> left(n, 0);
        st.clear();
        for (int k = 0; k < n - 1; ++k) {
            while (!st.empty() && a[st.back()] < a[k]) st.pop_back();
            int j = st.empty() ? -1 : st.back();
            left[k + 1] = a[k] * (k - j) + (j == -1 ? 0 : left[j + 1]);
            st.push_back(k);
        }

        // Rotated vessel i has edge a[i] immediately clockwise from it.
        vector<long long> answer(n);
        for (int i = 0; i < n; ++i) {
            int original = (peak + 1 + i) % n;
            answer[original] = right[i] + left[i];
        }

        for (int i = 0; i < n; ++i)
            cout << answer[i] << (i + 1 == n ? '\n' : ' ');
    }
    return 0;
}
