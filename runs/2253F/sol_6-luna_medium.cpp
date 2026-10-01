#include <bits/stdc++.h>
using namespace std;
static constexpr long long INF = (1LL << 62);

long long solve_component(int core, int n, const vector<int>& cost) {
    int width = 1;
    while (1LL * core * (1LL << width) <= n) ++width;
    int height = 1;
    long long p3 = 1;
    while (1LL * core * p3 * 3 <= n) p3 *= 3, ++height;
    vector<int> p2(width, 1), powers3(height, 1);
    for (int i = 1; i < width; ++i) p2[i] = 2 * p2[i - 1];
    for (int i = 1; i < height; ++i) powers3[i] = 3 * powers3[i - 1];

    int states = 1 << width, full = states - 1;
    vector<vector<vector<long long>>> dp(2, vector<vector<long long>>(width, vector<long long>(states, INF)));
    dp[0][0][0] = 0;
    for (int row = 0; row < height; ++row) {
        int cur_layer = row & 1, next_layer = cur_layer ^ 1;
        for (auto& v : dp[next_layer]) fill(v.begin(), v.end(), INF);
        for (int col = 0; col < width; ++col) {
            long long value = 1LL * core * p2[col] * powers3[row];
            long long remove_cost = value <= n ? cost[static_cast<int>(value)] : 0;
            int dest_col = col + 1;
            int dest_layer = cur_layer;
            if (dest_col == width) dest_col = 0, dest_layer = next_layer;
            for (int mask = 0; mask < states; ++mask) {
                long long cur = dp[cur_layer][col][mask];
                if (cur == INF) continue;
                bool can_keep = !(row > 0 && col + 2 < width &&
                    (mask & (1 << (width - 1))) && (mask & (1 << (width - 2))) &&
                    (mask & (1 << (width - 3))));
                int shifted = (mask << 1) & full;
                if (can_keep)
                    dp[dest_layer][dest_col][shifted | 1] = min(dp[dest_layer][dest_col][shifted | 1], cur);
                dp[dest_layer][dest_col][shifted] = min(dp[dest_layer][dest_col][shifted], cur + remove_cost);
            }
        }
    }
    return *min_element(dp[height & 1][0].begin(), dp[height & 1][0].end());
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    cin >> n;
    vector<int> cost(n + 1);
    for (int i = 1; i <= n; ++i) cin >> cost[i];
    long long ans = 0;
    for (int core = 1; core <= n; ++core)
        if (core % 2 && core % 3) ans += solve_component(core, n, cost);
    cout << ans << '\n';
}

