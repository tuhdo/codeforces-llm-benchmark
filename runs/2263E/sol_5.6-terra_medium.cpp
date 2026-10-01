#include <bits/stdc++.h>
using namespace std;

constexpr long long MOD = 1'000'000'007LL;

struct DSU {
    vector<int> p, sz;
    explicit DSU(int n) : p(n), sz(n, 1) { iota(p.begin(), p.end(), 0); }
    int find(int x) { return p[x] == x ? x : p[x] = find(p[x]); }
    void join(int a, int b) {
        a = find(a), b = find(b);
        if (a == b) return;
        if (sz[a] < sz[b]) swap(a, b);
        p[b] = a;
        sz[a] += sz[b];
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    vector<long long> fact(200001, 1);
    for (int i = 1; i <= 200000; ++i) fact[i] = fact[i - 1] * i % MOD;

    while (t--) {
        int n;
        cin >> n;
        vector<long long> a(n);
        unordered_map<long long, int> index;
        index.reserve(2 * n);
        index.max_load_factor(0.7);
        for (int i = 0; i < n; ++i) {
            cin >> a[i];
            index[a[i]] = i;
        }

        const long long high = a[n - 1];
        const long long second = a[n - 2];
        vector<vector<pair<int, int>>> graph(n); // neighbor, sum that creates the edge
        DSU dsu(n);

        auto add_matching = [&](long long sum) {
            int added = 0;
            for (int i = 0; i < n; ++i) {
                long long other_value = sum - a[i];
                auto it = index.find(other_value);
                if (it == index.end() || a[i] >= other_value) continue;
                int j = it->second;
                graph[i].push_back({j, static_cast<int>(sum == high)});
                graph[j].push_back({i, static_cast<int>(sum == high)});
                dsu.join(i, j);
                ++added;
            }
            return added;
        };

        int high_edges = add_matching(high);
        int second_edges = add_matching(second);

        bool ok = (high_edges + second_edges == n - 1);
        if (ok) {
            int root = dsu.find(0);
            for (int i = 1; i < n; ++i) {
                if (dsu.find(i) != root) {
                    ok = false;
                    break;
                }
            }
        }
        if (!ok) {
            cout << 0 << '\n';
            continue;
        }

        // The connected graph is a path. Walk from its endpoint a_n toward the
        // other endpoint. Each next vertex points back along this edge in the trip.
        int previous = -1;
        int current = n - 1;
        int left_count = 0;
        int right_count = 0;
        int seen = 1;
        while (true) {
            int next = -1;
            int is_high_edge = 0;
            for (auto [to, high_sum] : graph[current]) {
                if (to != previous) {
                    next = to;
                    is_high_edge = high_sum;
                    break;
                }
            }
            if (next == -1) break;
            if (is_high_edge) ++left_count;
            else ++right_count;
            previous = current;
            current = next;
            ++seen;
        }

        if (seen != n || right_count == 0) {
            cout << 0 << '\n';
            continue;
        }
        // Either orientation of this path gives a distinct family of island
        // orders. Within either family, the two blocks can be permuted freely.
        cout << 2LL * fact[left_count] % MOD * fact[right_count - 1] % MOD << '\n';
    }
}
