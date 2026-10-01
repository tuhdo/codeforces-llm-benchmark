#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long n, a, b;
    if (!(cin >> n >> a >> b)) return 0;

    auto factorize = [](long long x) {
        vector<pair<long long, int>> f;
        for (long long p = 2; p * p <= x; ++p) {
            if (x % p == 0) {
                int e = 0;
                do { x /= p; ++e; } while (x % p == 0);
                f.push_back({p, e});
            }
        }
        if (x > 1) f.push_back({x, 1});
        return f;
    };

    auto fa = factorize(a), fb = factorize(b);
    map<long long, int> exponents;
    for (auto [p, e] : fa) exponents[p] = max(exponents[p], e);
    for (auto [p, e] : fb) exponents[p] = max(exponents[p], e);

    vector<long long> divisors{1};
    for (auto [p, e] : exponents) {
        vector<long long> next;
        long long power = 1;
        for (int k = 0; k <= e; ++k) {
            for (long long d : divisors) {
                if (d <= n / power) next.push_back(d * power);
            }
            if (k < e) {
                if (power > n / p) break;
                power *= p;
            }
        }
        divisors.swap(next);
    }
    sort(divisors.begin(), divisors.end());
    divisors.erase(unique(divisors.begin(), divisors.end()), divisors.end());

    unordered_map<long long, int> id;
    id.reserve(divisors.size() * 2);
    for (int i = 0; i < (int)divisors.size(); ++i) id[divisors[i]] = i;
    int s = id[a], t = id[b], m = divisors.size();

    const long long INF = (1LL << 62);
    vector<long long> dist(m, INF);
    dist[s] = 0;

    // Dijkstra on the complete graph of candidate values. The queue lets us
    // stop as soon as the destination is settled.
    priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<>> pq;
    pq.push({0, s});
    while (!pq.empty()) {
        auto [dv, v] = pq.top();
        pq.pop();
        if (dv != dist[v]) continue;
        if (v == t) break;
        for (int u = 0; u < m; ++u) {
            long long w = max(divisors[v], divisors[u]) / gcd(divisors[v], divisors[u]);
            if (dist[u] > dv + w) {
                dist[u] = dv + w;
                pq.push({dist[u], u});
            }
        }
    }

    cout << dist[t] << '\n';
    return 0;
}
