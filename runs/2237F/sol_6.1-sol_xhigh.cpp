#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n, m;
        cin >> n >> m;

        // Valid arrays start with 1, end with m, and allow transitions
        // x -> x + 1, x -> 1, or m -> any value.
        // A state stores the maximum matches when the last value is j.
        // Its actual score is max(score[slot], baseline); -1 is unreachable.
        vector<int> score(m, -1);
        int first = 0;
        int baseline = -1;
        int best = 0;

        for (int i = 0; i < n; ++i) {
            int a;
            cin >> a;

            // Rotate j -> j + 1. The old m slot becomes the new 1 slot.
            first = (first == 0 ? m - 1 : first - 1);
            baseline = max(baseline, score[first]);
            score[first] = best;

            int slot = first + a - 1;
            if (slot >= m) slot -= m;
            int current = max(score[slot], baseline);
            if (current >= 0) {
                score[slot] = current + 1;
                best = max(best, score[slot]);
            }
        }

        int last = (first == 0 ? m - 1 : first - 1);
        cout << n - max(score[last], baseline) << '\n';
    }
}
