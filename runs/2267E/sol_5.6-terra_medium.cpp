#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCases;
    cin >> testCases;
    while (testCases--) {
        int n, q;
        cin >> n >> q;
        string s;
        cin >> s;

        long long zeroes = count(s.begin(), s.end(), '0');
        long long ones = n - zeroes;
        long long transitionSum = 0;
        for (int boundary = 1; boundary < n; ++boundary) {
            if (s[boundary - 1] != s[boundary]) {
                transitionSum += 1LL * boundary * (n - boundary);
            }
        }

        auto answer = [&]() {
            return (transitionSum + zeroes * ones) / 2;
        };

        cout << answer();
        while (q--) {
            int position;
            cin >> position;
            --position;

            auto updateBoundary = [&](int boundary) {
                if (boundary <= 0 || boundary >= n) return;
                long long weight = 1LL * boundary * (n - boundary);
                if (s[boundary - 1] != s[boundary]) transitionSum -= weight;
                else transitionSum += weight;
            };

            // Flipping either endpoint toggles whether each adjacent boundary
            // is a transition, so update their contributions before the flip.
            updateBoundary(position);
            updateBoundary(position + 1);

            if (s[position] == '0') {
                --zeroes;
                ++ones;
                s[position] = '1';
            } else {
                ++zeroes;
                --ones;
                s[position] = '0';
            }
            cout << ' ' << answer();
        }
        cout << '\n';
    }
    return 0;
}
