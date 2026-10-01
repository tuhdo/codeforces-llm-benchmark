#include <bits/stdc++.h>
using namespace std;

static constexpr long long MOD = 1'000'000'007LL;
static constexpr int MAXN = 500000;

long long mod_pow(long long a, long long e) {
    long long r = 1;
    while (e > 0) {
        if (e & 1) r = r * a % MOD;
        a = a * a % MOD;
        e >>= 1;
    }
    return r;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<long long> fact(MAXN + 1), inv_fact(MAXN + 1);
    fact[0] = 1;
    for (int i = 1; i <= MAXN; ++i) fact[i] = fact[i - 1] * i % MOD;
    inv_fact[MAXN] = mod_pow(fact[MAXN], MOD - 2);
    for (int i = MAXN; i > 0; --i) {
        inv_fact[i - 1] = inv_fact[i] * i % MOD;
    }

    auto choose = [&](int n, int k) -> long long {
        if (k < 0 || k > n) return 0;
        return fact[n] * inv_fact[k] % MOD * inv_fact[n - k] % MOD;
    };

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        vector<long long> a(n + 1);
        long long sum = 0;
        for (int i = 1; i <= n; ++i) {
            cin >> a[i];
            sum += a[i];
        }

        if (sum != 1LL * n * (n + 1) / 2) {
            cout << 0 << '\n';
            continue;
        }

        struct Frame {
            int l, r;
            int stage = 0;
            int root = 0;
            long long left_answer = 1;
        };

        vector<Frame> st;
        st.reserve(2 * n + 1);
        st.push_back({1, n});
        long long last = 1;

        while (!st.empty()) {
            Frame &f = st.back();

            if (f.stage == 0) {
                if (f.l > f.r) {
                    last = 1;
                    st.pop_back();
                    continue;
                }

                int i = f.l, j = f.r;
                int root = 0;
                while (i <= j) {
                    if (a[i] == 1LL * (i - f.l + 1) * (f.r - i + 1)) {
                        root = i;
                        break;
                    }
                    ++i;
                    if (i <= j) {
                        if (a[j] == 1LL * (j - f.l + 1) * (f.r - j + 1)) {
                            root = j;
                            break;
                        }
                        --j;
                    }
                }

                if (root == 0) {
                    last = 0;
                    st.pop_back();
                    continue;
                }

                f.root = root;
                f.stage = 1;
                st.push_back({f.l, root - 1});
            } else if (f.stage == 1) {
                f.left_answer = last;
                f.stage = 2;
                st.push_back({f.root + 1, f.r});
            } else {
                long long right_answer = last;
                last = f.left_answer * right_answer % MOD;
                last = last * choose(f.r - f.l, f.root - f.l) % MOD;
                st.pop_back();
            }
        }

        cout << last << '\n';
    }
    return 0;
}
