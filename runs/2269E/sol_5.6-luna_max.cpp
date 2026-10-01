#include <bits/stdc++.h>
using namespace std;

struct Query {
    int hi;
    int value;
    int next;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;

    constexpr int B = 18;
    constexpr int VALUE_COUNT = 1 << B;

    while (t--) {
        int n;
        cin >> n;

        vector<int> a(n);
        for (int &x : a) cin >> x;

        vector<int> pref(n + 1, 0);
        for (int i = 0; i < n; ++i) {
            pref[i + 1] = pref[i] ^ a[i];
        }

        // prev_ge[i] is the closest position to the left with a value >= a[i].
        // next_greater[i] is the closest position to the right with a value > a[i].
        // Together they assign every subarray to its leftmost maximum.
        vector<int> prev_ge(n), next_greater(n), st;
        st.reserve(n);

        for (int i = 0; i < n; ++i) {
            while (!st.empty() && a[st.back()] < a[i]) st.pop_back();
            prev_ge[i] = st.empty() ? -1 : st.back();
            st.push_back(i);
        }

        st.clear();
        for (int i = n - 1; i >= 0; --i) {
            while (!st.empty() && a[st.back()] <= a[i]) st.pop_back();
            next_greater[i] = st.empty() ? n : st.back();
            st.push_back(i);
        }

        vector<int> head(n + 1, -1);
        vector<Query> queries;
        vector<int> next_position(VALUE_COUNT);
        vector<int> position_version(VALUE_COUNT, 0);
        int version = 0;

        auto feasible = [&](int mask) {
            fill(head.begin(), head.end(), -1);
            queries.clear();

            auto add_query = [&](int lo, int hi, int value) {
                queries.push_back({hi, value, head[lo]});
                head[lo] = static_cast<int>(queries.size()) - 1;
            };

            for (int k = 0; k < n; ++k) {
                if ((a[k] & mask) != mask) continue;

                // Segments assigned to k that start before k have prefix-XOR
                // endpoints i in [prev_ge[k]+1, k-1] and j in
                // [k+1, next_greater[k]].
                int left_lo = prev_ge[k] + 1;
                int left_hi = k - 1;
                int right_lo = k + 1;
                int right_hi = next_greater[k];

                int left_size = max(0, left_hi - left_lo + 1);
                int right_size = max(0, right_hi - right_lo + 1);

                // Enumerate the shorter side.  Each query asks whether the
                // complementary projected prefix-XOR occurs on the other side.
                if (left_size != 0 && right_size != 0) {
                    if (left_size <= right_size) {
                        for (int i = left_lo; i <= left_hi; ++i) {
                            int wanted = mask ^ (pref[i] & mask);
                            add_query(right_lo, right_hi, wanted);
                        }
                    } else {
                        for (int j = right_lo; j <= right_hi; ++j) {
                            int wanted = mask ^ (pref[j] & mask);
                            add_query(left_lo, left_hi, wanted);
                        }
                    }
                }

                // Segments assigned to k that start exactly at k must have
                // length at least two, so j starts at k+2 here.
                int extra_lo = k + 2;
                int extra_hi = next_greater[k];
                if (extra_lo <= extra_hi) {
                    int wanted = mask ^ (pref[k] & mask);
                    add_query(extra_lo, extra_hi, wanted);
                }
            }

            ++version;
            for (int lo = n; lo >= 0; --lo) {
                int key = pref[lo] & mask;
                position_version[key] = version;
                next_position[key] = lo;
                for (int id = head[lo]; id != -1; id = queries[id].next) {
                    const Query &q = queries[id];
                    if (position_version[q.value] == version &&
                        next_position[q.value] <= q.hi) {
                        return true;
                    }
                }
            }
            return false;
        };

        int answer = 0;
        for (int bit = B - 1; bit >= 0; --bit) {
            int candidate = answer | (1 << bit);
            if (feasible(candidate)) answer = candidate;
        }

        cout << answer << '\n';
    }

    return 0;
}
