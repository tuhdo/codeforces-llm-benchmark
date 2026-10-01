#include <bits/stdc++.h>
using namespace std;

static constexpr long long INF = (1LL << 62);

int width_of(int x) {
    return static_cast<int>(bit_width(static_cast<unsigned int>(x)));
}

void build_mask_costs(vector<long long>& mask_cost, int width, int row_base,
                      const vector<long long>& removal_cost) {
    const int count = 1 << width;
    mask_cost[0] = 0;
    for (int mask = 1; mask < count; ++mask) {
        const int bit = countr_zero(static_cast<unsigned int>(mask));
        const int value = row_base * (1 << bit);
        mask_cost[mask] = mask_cost[mask & (mask - 1)] + removal_cost[value];
    }
}

long long solve_component(int core, int n, const vector<long long>& removal_cost,
                          vector<long long>& dp, vector<long long>& next_dp,
                          vector<long long>& mask_cost, vector<long long>& best) {
    int row_base = core;
    int bound = n / row_base;
    int width = width_of(bound);
    int count = 1 << width;

    build_mask_costs(mask_cost, width, row_base, removal_cost);
    copy(mask_cost.begin(), mask_cost.begin() + count, dp.begin());

    // A constraint starting in this row exists for every a with 2^a <= bound / 4.
    while (bound >= 4) {
        const int starts = width_of(bound / 4);
        const int req_count = 1 << starts;
        const int req_mask = req_count - 1;
        const int full_mask = count - 1;
        fill(best.begin(), best.begin() + req_count, INF);

        // For a previous-row deletion mask, req contains each a where
        // positions a, a+1, and a+2 are all still present.
        for (int mask = 0; mask < count; ++mask) {
            const int absent = full_mask ^ mask;
            const int req = absent & (absent >> 1) & (absent >> 2) & req_mask;
            best[req] = min(best[req], dp[mask]);
        }

        // best[mask] becomes the minimum over all submasks of mask.
        for (int bit = 0; bit < starts; ++bit) {
            const int bit_mask = 1 << bit;
            for (int mask = 0; mask < req_count; ++mask) {
                if (mask & bit_mask) {
                    best[mask] = min(best[mask], best[mask ^ bit_mask]);
                }
            }
        }

        const int next_bound = bound / 3;
        const int next_width = width_of(next_bound);
        const int next_count = 1 << next_width;
        const int next_base = row_base * 3;
        build_mask_costs(mask_cost, next_width, next_base, removal_cost);

        for (int mask = 0; mask < next_count; ++mask) {
            next_dp[mask] = best[mask & req_mask] + mask_cost[mask];
        }

        dp.swap(next_dp);
        row_base = next_base;
        bound = next_bound;
        width = next_width;
        count = next_count;
    }

    return *min_element(dp.begin(), dp.begin() + count);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    vector<long long> removal_cost(n + 1);
    for (int i = 1; i <= n; ++i) {
        cin >> removal_cost[i];
    }

    const int max_width = width_of(n);
    const int max_states = 1 << max_width;
    vector<long long> dp(max_states), next_dp(max_states), mask_cost(max_states), best(max_states);

    long long answer = 0;
    for (int core = 1; core <= n / 4; ++core) {
        if (core % 2 == 0 || core % 3 == 0) {
            continue;
        }
        answer += solve_component(core, n, removal_cost, dp, next_dp, mask_cost, best);
    }

    cout << answer << '\n';
    return 0;
}
