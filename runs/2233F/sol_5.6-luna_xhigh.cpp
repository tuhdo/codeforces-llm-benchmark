#include <bits/stdc++.h>
using namespace std;

using u64 = uint64_t;
struct GraphBuilder {
    static constexpr int MAXP = 20;

    vector<u64> primes;
    vector<int> max_exp;
    unordered_set<u64> lcm_primes;
    vector<u64> vertices;
    vector<array<unsigned char, MAXP>> exponents;
    unordered_map<u64, int> id;

    vector<vector<int>> adjacency;

    u64 current_y = 0;
    int current_y_id = -1;
    array<unsigned char, MAXP> current_y_exp{};
    array<unsigned char, MAXP> current_g_exp{};

    void emit_edge(u64 x) {
        if (x == current_y) return;
        auto it = id.find(x);
        if (it == id.end()) return;
        int u = it->second;
        int v = current_y_id;
        adjacency[u].push_back(v);
        adjacency[v].push_back(u);
    }

    void enumerate_r(int pos, int prime_pos, u64 bound, u64 g, u64 r) {
        if (r >= bound) return;
        if (pos == static_cast<int>(primes.size())) {
            emit_edge(g * r);
            return;
        }
        if (pos == prime_pos) {
            enumerate_r(pos + 1, prime_pos, bound, g, r);
            return;
        }

        int available = max_exp[pos] - static_cast<int>(current_g_exp[pos]);
        u64 value = r;
        for (int e = 0; e <= available; ++e) {
            enumerate_r(pos + 1, prime_pos, bound, g, value);
            if (e == available || value > (bound - 1) / primes[pos]) break;
            value *= primes[pos];
        }
    }

    void process_pair(u64 g) {
        u64 s = current_y / g;
        int total_exponent = 0;
        int first_prime = -1;
        for (int i = 0; i < static_cast<int>(primes.size()); ++i) {
            int difference = static_cast<int>(current_y_exp[i]) -
                             static_cast<int>(current_g_exp[i]);
            total_exponent += difference;
            if (difference > 0 && first_prime == -1) first_prime = i;
        }
        if (total_exponent == 0) return;

        if (total_exponent == 1) {
            // The reduced larger endpoint is a prime p.  Every divisor r<p
            // of L/g not containing p gives one primitive edge.
            enumerate_r(0, first_prime, primes[first_prime], g, 1);
            return;
        }

        // For composite s, the only edges worth keeping have prime r and
        // satisfy r=s-d with 1<=d<spf(s).  The remaining edges are
        // replaceable by a cheaper path through divisors of the lcm.
        u64 smallest_prime = primes[first_prime];
        for (u64 d = 1; d < smallest_prime; ++d) {
            u64 r = s - d;
            if (s % r == 0) continue;
            if (!lcm_primes.contains(r)) continue;
            emit_edge(g * r);
        }
    }

    void enumerate_g(int pos, u64 g) {
        if (pos == static_cast<int>(primes.size())) {
            process_pair(g);
            return;
        }
        u64 value = g;
        int available = current_y_exp[pos];
        for (int e = 0; e <= available; ++e) {
            current_g_exp[pos] = static_cast<unsigned char>(e);
            enumerate_g(pos + 1, value);
            if (e != available) value *= primes[pos];
        }
    }

    void generate_edges() {
        for (int yi = 0; yi < static_cast<int>(vertices.size()); ++yi) {
            current_y = vertices[yi];
            current_y_id = yi;
            current_y_exp = exponents[yi];
            current_g_exp.fill(0);
            enumerate_g(0, 1);
        }
    }

    void build() {
        adjacency.resize(vertices.size());
        generate_edges();
    }
};

static vector<pair<u64, int>> factor_number(u64 x) {
    vector<pair<u64, int>> result;
    for (u64 p = 2; p * p <= x; ++p) {
        if (x % p != 0) continue;
        int e = 0;
        do {
            x /= p;
            ++e;
        } while (x % p == 0);
        result.push_back({p, e});
    }
    if (x > 1) result.push_back({x, 1});
    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    u64 n, a, b;
    if (!(cin >> n >> a >> b)) return 0;

    auto fa = factor_number(a);
    auto fb = factor_number(b);
    map<u64, int> merged;
    for (auto [p, e] : fa) merged[p] = max(merged[p], e);
    for (auto [p, e] : fb) merged[p] = max(merged[p], e);

    GraphBuilder graph;
    for (auto [p, e] : merged) {
        graph.primes.push_back(p);
        graph.max_exp.push_back(e);
        graph.lcm_primes.insert(p);
    }
    function<void(int, u64)> make_divisors = [&](int pos, u64 value) {
        if (pos == static_cast<int>(graph.primes.size())) {
            if (value <= n) graph.vertices.push_back(value);
            return;
        }
        u64 next = value;
        for (int e = 0; e <= graph.max_exp[pos]; ++e) {
            make_divisors(pos + 1, next);
            if (e != graph.max_exp[pos]) next *= graph.primes[pos];
        }
    };
    make_divisors(0, 1);
    sort(graph.vertices.begin(), graph.vertices.end());

    graph.exponents.resize(graph.vertices.size());
    for (size_t i = 0; i < graph.vertices.size(); ++i) {
        u64 value = graph.vertices[i];
        graph.exponents[i].fill(0);
        for (int j = 0; j < static_cast<int>(graph.primes.size()); ++j) {
            while (value % graph.primes[j] == 0) {
                value /= graph.primes[j];
                ++graph.exponents[i][j];
            }
        }
    }

    graph.id.reserve(graph.vertices.size() * 2 + 1);
    for (int i = 0; i < static_cast<int>(graph.vertices.size()); ++i) {
        graph.id.emplace(graph.vertices[i], i);
    }

    graph.build();

    int start = graph.id.at(a);
    int target = graph.id.at(b);
    const long long INF = (1LL << 62);
    vector<long long> distance(graph.vertices.size(), INF);
    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>>
        pq;
    distance[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [du, u] = pq.top();
        pq.pop();
        if (du != distance[u]) continue;
        if (u == target) break;
        for (int v : graph.adjacency[u]) {
            u64 x = graph.vertices[u];
            u64 y = graph.vertices[v];
            u64 weight = max(x, y) / std::gcd(x, y);
            if (du + static_cast<long long>(weight) < distance[v]) {
                distance[v] = du + static_cast<long long>(weight);
                pq.push({distance[v], v});
            }
        }
    }

    cout << distance[target] << '\n';
    return 0;
}
