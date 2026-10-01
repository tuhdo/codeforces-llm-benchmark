#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
using i128 = __int128_t;

struct Factor {
    int p;
    int e;
};

static vector<Factor> factorize(int64 x) {
    vector<Factor> result;
    for (int64 p = 2; p * p <= x; ++p) {
        if (x % p != 0) continue;
        int e = 0;
        do {
            x /= p;
            ++e;
        } while (x % p == 0);
        result.push_back({static_cast<int>(p), e});
    }
    if (x > 1) result.push_back({static_cast<int>(x), 1});
    return result;
}

static vector<int64> make_divisors(const vector<Factor>& factors) {
    vector<int64> divisors = {1};
    for (const auto& [p, e] : factors) {
        const int old_size = static_cast<int>(divisors.size());
        int64 power = 1;
        for (int k = 1; k <= e; ++k) {
            power *= p;
            for (int i = 0; i < old_size; ++i) {
                divisors.push_back(divisors[i] * power);
            }
        }
    }
    sort(divisors.begin(), divisors.end());
    return divisors;
}

static bool is_prime_power(int64 x) {
    if (x == 1) return true;
    for (int64 d = 2; d * d <= x; ++d) {
        if (x % d == 0) {
            while (x % d == 0) x /= d;
            return x == 1;
        }
    }
    return true;
}

struct DivisorData {
    vector<int64> values;
    unordered_map<int64, int> index;
    vector<vector<int>> divisors;
    vector<vector<int>> prime_power_divisors;
};

static DivisorData build_data(const vector<Factor>& factors) {
    DivisorData data;
    data.values = make_divisors(factors);
    data.index.reserve(data.values.size() * 2 + 1);
    for (int i = 0; i < static_cast<int>(data.values.size()); ++i) {
        data.index[data.values[i]] = i;
    }

    const int m = static_cast<int>(data.values.size());
    data.divisors.resize(m);
    data.prime_power_divisors.resize(m);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < m; ++j) {
            if (data.values[i] % data.values[j] != 0) continue;
            data.divisors[i].push_back(j);
            if (data.values[j] > 1 && is_prime_power(data.values[j])) {
                data.prime_power_divisors[i].push_back(j);
            }
        }
    }
    return data;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int64 n, a, b;
    cin >> n >> a >> b;

    const int64 g = gcd(a, b);
    const int64 x = a / g;
    const int64 y = b / g;

    DivisorData dx = build_data(factorize(x));
    DivisorData dy = build_data(factorize(y));

    const int nx = static_cast<int>(dx.values.size());
    const int ny = static_cast<int>(dy.values.size());
    const int64 state_count = 1LL * nx * ny;
    const int start_x = dx.index[x];
    const int start_y = dy.index[1];
    const int target_x = dx.index[1];
    const int target_y = dy.index[y];
    const int start = start_x * ny + start_y;
    const int target = target_x * ny + target_y;

    const int64 INF = numeric_limits<int64>::max() / 4;
    vector<int64> dist(static_cast<size_t>(state_count), INF);
    priority_queue<pair<int64, int>, vector<pair<int64, int>>, greater<pair<int64, int>>> pq;
    dist[start] = 0;
    pq.push({0, start});

    auto relax = [&](int state, int next_x, int next_y, int64 edge_cost) {
        const i128 value = static_cast<i128>(g) * dx.values[next_x] * dy.values[next_y];
        if (value > n) return;
        const int next_state = next_x * ny + next_y;
        const int64 candidate = dist[state] + edge_cost;
        if (candidate >= dist[next_state]) return;
        dist[next_state] = candidate;
        pq.push({candidate, next_state});
    };

    while (!pq.empty()) {
        const auto [current_distance, state] = pq.top();
        pq.pop();
        if (current_distance != dist[state]) continue;
        if (state == target) break;

        const int ix = state / ny;
        const int iy = state % ny;
        const int64 remaining_x = dx.values[ix];
        const int64 added_y = dy.values[iy];
        const int iy_remaining = dy.index[y / added_y];

        // Remove any divisor by itself, or add any divisor by itself.
        for (int ux : dx.divisors[ix]) {
            if (ux == dx.index[1]) continue;
            const int next_ix = dx.index[remaining_x / dx.values[ux]];
            relax(state, next_ix, iy, dx.values[ux]);
        }
        for (int vy : dy.divisors[iy_remaining]) {
            if (vy == dy.index[1]) continue;
            const int next_iy = dy.index[added_y * dy.values[vy]];
            relax(state, ix, next_iy, dy.values[vy]);
        }

        // Every useful remaining edge has a prime-power reduced side.
        for (int ux : dx.prime_power_divisors[ix]) {
            const int next_ix = dx.index[remaining_x / dx.values[ux]];
            for (int vy : dy.divisors[iy_remaining]) {
                if (vy == dy.index[1]) continue;
                const int next_iy = dy.index[added_y * dy.values[vy]];
                relax(state, next_ix, next_iy,
                      max(dx.values[ux], dy.values[vy]));
            }
        }
        for (int vy : dy.prime_power_divisors[iy_remaining]) {
            const int next_iy = dy.index[added_y * dy.values[vy]];
            for (int ux : dx.divisors[ix]) {
                if (ux == dx.index[1]) continue;
                const int next_ix = dx.index[remaining_x / dx.values[ux]];
                relax(state, next_ix, next_iy,
                      max(dx.values[ux], dy.values[vy]));
            }
        }
    }

    cout << dist[target] << '\n';
    return 0;
}
