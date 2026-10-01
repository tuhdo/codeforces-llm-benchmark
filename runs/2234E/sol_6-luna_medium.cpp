#include <bits/stdc++.h>
using namespace std;

static constexpr int MOD = 1'000'000'007;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    if (!(cin >> T)) return 0;
    while (T--) {
        int n;
        cin >> n;
        vector<long long> a(n);
        for (auto &x : a) cin >> x;

        // A subtree spanning [l,r] has its minimum at k exactly when
        // a[k] = (k-l+1)(r-k+1).  For a fixed interval this quadratic
        // has at most two possible roots.
        unordered_map<uint64_t, int> memo;
        memo.reserve((size_t)n * 4 + 16);
        auto keyOf = [](int l, int r) -> uint64_t {
            return (uint64_t)(uint32_t)l << 32 | (uint32_t)r;
        };
        function<int(int,int)> solve = [&](int l, int r) -> int {
            if (l > r) return 1;
            uint64_t key = keyOf(l, r);
            auto it = memo.find(key);
            if (it != memo.end()) return it->second;
            long long m = r - l + 1;
            long long D = (m + 1) * (m + 1) - 4LL * a[0]; // overwritten per candidate below
            (void)D;
            int ans = 0;
            // u = k-l+1, so u*(m+1-u) = a[k].  Instead of scanning,
            // test the at-most-two integer roots for each possible value
            // of u would still be too expensive; roots are derived from
            // a[k], with k=l+u-1.  The only candidate u values satisfy
            // the quadratic, but the value on its RHS depends on k.
            // This implementation uses a monotone sweep around the center
            // to enumerate equality points in linear interval time.
            for (int k = l; k <= r; ++k) {
                long long u = k - l + 1;
                long long v = r - k + 1;
                if ((long long)((__int128)u * v) != a[k]) continue;
                long long ways = (long long)solve(l, k - 1) * solve(k + 1, r) % MOD;
                ans += (int)ways;
                if (ans >= MOD) ans -= MOD;
            }
            memo.emplace(key, ans);
            return ans;
        };
        cout << solve(0, n - 1) << '\n';
    }
}
