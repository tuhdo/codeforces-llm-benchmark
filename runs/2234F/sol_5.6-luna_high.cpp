#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        vector<long long> h(n);
        for (long long &x : h) cin >> x;

        // Remove one globally maximum edge.  The remaining cycle is a path;
        // its maximum edge on the path between two vertices is their optimal
        // bottleneck value in the original cycle.
        int cut = int(max_element(h.begin(), h.end()) - h.begin());
        vector<long long> edge(n - 1);
        vector<int> vertex(n);

        // Path order starts just after the removed edge and follows the cycle.
        for (int p = 0; p < n; ++p) {
            vertex[p] = (cut + 1 + p) % n;
        }
        for (int p = 0; p < n - 1; ++p) {
            edge[p] = h[(cut + 1 + p) % n];
        }

        const int m = n - 1;
        vector<int> next_greater(m, m), previous_greater(m, -1);
        vector<int> st;

        // First strictly greater edge to the right.
        for (int i = m - 1; i >= 0; --i) {
            while (!st.empty() && edge[st.back()] <= edge[i]) st.pop_back();
            if (!st.empty()) next_greater[i] = st.back();
            st.push_back(i);
        }

        st.clear();
        // First strictly greater edge to the left.
        for (int i = 0; i < m; ++i) {
            while (!st.empty() && edge[st.back()] <= edge[i]) st.pop_back();
            if (!st.empty()) previous_greater[i] = st.back();
            st.push_back(i);
        }

        // right_sum[p] = sum of bottleneck values from path vertex p to the
        // vertices strictly to its right.
        vector<long long> right_sum(n, 0), left_sum(n, 0);
        for (int i = m - 1; i >= 0; --i) {
            int j = next_greater[i];
            if (j == m) right_sum[i] = 1LL * (m - i) * edge[i];
            else right_sum[i] = 1LL * (j - i) * edge[i] + right_sum[j];
        }

        // left_sum[p] = sum of bottleneck values from path vertex p to the
        // vertices strictly to its left.
        for (int k = 0; k < m; ++k) {
            int p = previous_greater[k];
            if (p == -1) left_sum[k + 1] = 1LL * (k + 1) * edge[k];
            else left_sum[k + 1] = left_sum[p + 1] + 1LL * (k - p) * edge[k];
        }

        vector<long long> answer(n);
        for (int p = 0; p < n; ++p) {
            answer[vertex[p]] = left_sum[p] + right_sum[p];
        }

        for (long long x : answer) cout << x << ' ';
        cout << '\n';
    }
    return 0;
}
