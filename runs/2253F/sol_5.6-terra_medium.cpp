#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
constexpr int64 INF = (1LL << 62);

int n;
vector<int> removal_cost;

// c[row][column] is the cost of deleting core * 3^row * 2^column.
// A zero denotes a point outside [1, n].
int64 solve_component(const vector<vector<int>>& c) {
    const int rows = static_cast<int>(c.size());
    const int cols = static_cast<int>(c[0].size());
    const int mask_count = 1 << cols;
    const int full_mask = mask_count - 1;

    // The mask stores the latest `cols` choices while scanning left to right.
    // A one means the corresponding number is kept. The extra cursor dimension
    // distinguishes the states before each column of the current row.
    vector<vector<vector<int64>>> dp(
        2, vector<vector<int64>>(cols, vector<int64>(mask_count, INF)));
    dp[0][0][0] = 0;

    for (int row = 0; row < rows; ++row) {
        const int current = row & 1;
        const int next = current ^ 1;
        for (int col = 0; col < cols; ++col) {
            fill(dp[next][col].begin(), dp[next][col].end(), INF);
        }

        for (int col = 0; col < cols; ++col) {
            int target_layer = current;
            int target_col = col + 1;
            if (target_col == cols) {
                target_layer = next;
                target_col = 0;
            }
            for (int mask = 0; mask < mask_count; ++mask) {
                const int64 value = dp[current][col][mask];
                if (value == INF) continue;

                // Keep this point. This completes a forbidden shape only if
                // the three preceding points in the row above are all kept.
                const bool completes_forbidden =
                    row > 0 && col + 2 < cols &&
                    (mask & (1 << (cols - 1))) &&
                    (mask & (1 << (cols - 2))) &&
                    (mask & (1 << (cols - 3)));
                if (!completes_forbidden) {
                    const int kept_mask = ((mask << 1) & full_mask) | 1;
                    dp[target_layer][target_col][kept_mask] =
                        min(dp[target_layer][target_col][kept_mask], value);
                }

                // Delete this point.
                const int deleted_mask = (mask << 1) & full_mask;
                dp[target_layer][target_col][deleted_mask] = min(
                    dp[target_layer][target_col][deleted_mask], value + c[row][col]);
            }
        }
    }

    return *min_element(dp[rows & 1][0].begin(), dp[rows & 1][0].end());
}

int64 component_cost(int core) {
    vector<int> pow2{1}, pow3{1};
    while (1LL * core * pow2.back() * 2 <= n) pow2.push_back(pow2.back() * 2);
    while (1LL * core * pow3.back() * 3 <= n) pow3.push_back(pow3.back() * 3);

    vector<vector<int>> c(pow3.size(), vector<int>(pow2.size()));
    for (int row = 0; row < static_cast<int>(pow3.size()); ++row) {
        for (int col = 0; col < static_cast<int>(pow2.size()); ++col) {
            const int64 number = 1LL * core * pow3[row] * pow2[col];
            if (number <= n) c[row][col] = removal_cost[number];
        }
    }
    return solve_component(c);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> n;
    removal_cost.resize(n + 1);
    for (int i = 1; i <= n; ++i) cin >> removal_cost[i];

    int64 answer = 0;
    for (int core = 1; core <= n; ++core) {
        if (core % 2 != 0 && core % 3 != 0) {
            answer += component_cost(core);
        }
    }
    cout << answer << '\n';
}
