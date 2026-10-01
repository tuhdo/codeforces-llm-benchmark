#include <bits/stdc++.h>
using namespace std;

constexpr long long MOD = 1'000'000'007;

bool forms_path(const vector<long long>& a, long long r) {
    const int n = static_cast<int>(a.size());
    const long long maximum = a.back();

    vector<char> visited(n, false);
    long long current = maximum;
    visited[n - 1] = true;

    for (int step = 0; step + 1 < n; ++step) {
        const long long sum = (step % 2 == 0 ? maximum : r);
        const long long next = sum - current;
        const auto it = lower_bound(a.begin(), a.end(), next);

        if (next == current || it == a.end() || *it != next) return false;

        const int index = static_cast<int>(it - a.begin());
        if (visited[index]) return false;

        visited[index] = true;
        current = next;
    }

    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    constexpr int MAX_N = 200'000;
    vector<long long> factorial(MAX_N + 1, 1);
    for (int i = 1; i <= MAX_N; ++i) {
        factorial[i] = factorial[i - 1] * i % MOD;
    }

    int test_cases;
    cin >> test_cases;

    while (test_cases--) {
        int n;
        cin >> n;

        vector<long long> a(n);
        for (long long& value : a) cin >> value;

        int valid_choices = forms_path(a, a[n - 2]);
        if (n % 2 == 0) {
            valid_choices += forms_path(a, a[n - 3]);
        }

        long long answer = valid_choices;
        answer = answer * factorial[n / 2] % MOD;
        answer = answer * factorial[(n - 3) / 2] % MOD;
        cout << answer << '\n';
    }
}
