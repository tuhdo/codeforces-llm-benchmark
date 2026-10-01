#include <bits/stdc++.h>
using namespace std;

class SegmentTree {
    int n_;
    vector<int> tree_;

public:
    explicit SegmentTree(int n) : n_(n), tree_(4 * n + 4, 0) {}

    void set_value(int node, int left, int right, int pos, int value) {
        if (left == right) {
            tree_[node] = value;
            return;
        }
        int mid = (left + right) / 2;
        if (pos <= mid) set_value(node * 2, left, mid, pos, value);
        else set_value(node * 2 + 1, mid + 1, right, pos, value);
        tree_[node] = min(tree_[node * 2], tree_[node * 2 + 1]);
    }

    void set_value(int pos, int value) { set_value(1, 1, n_, pos, value); }

    int minimum(int node, int left, int right, int query_left, int query_right) const {
        if (query_left <= left && right <= query_right) return tree_[node];
        int mid = (left + right) / 2;
        int result = INT_MAX;
        if (query_left <= mid) result = min(result, minimum(node * 2, left, mid, query_left, query_right));
        if (query_right > mid) result = min(result, minimum(node * 2 + 1, mid + 1, right, query_left, query_right));
        return result;
    }

    int minimum(int left, int right) const {
        if (left > right) return INT_MAX;
        return minimum(1, 1, n_, left, right);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    constexpr int LIMIT = 400000;
    vector<int> smallest_prime_factor(LIMIT + 1);
    vector<int> primes;
    for (int value = 2; value <= LIMIT; ++value) {
        if (smallest_prime_factor[value] == 0) {
            smallest_prime_factor[value] = value;
            primes.push_back(value);
        }
        for (int prime : primes) {
            if (prime > smallest_prime_factor[value] || value * prime > LIMIT) break;
            smallest_prime_factor[value * prime] = prime;
        }
    }

    vector<int> prime_powers;
    for (int prime : primes) {
        long long power = prime;
        while (power <= LIMIT) {
            prime_powers.push_back(static_cast<int>(power));
            power *= prime;
        }
    }
    sort(prime_powers.begin(), prime_powers.end());

    int test_cases;
    cin >> test_cases;
    while (test_cases--) {
        int n;
        cin >> n;
        vector<int> a(n + 1);
        for (int i = 1; i <= n; ++i) cin >> a[i];

        int bound = *upper_bound(prime_powers.begin(), prime_powers.end(), n);
        vector<int> candidates;
        for (int power : prime_powers) {
            if (power > bound) break;
            candidates.push_back(power);
        }

        // At coordinate q (a prime power), store the latest index containing a multiple of q.
        // Other coordinates must not influence a minimum query.
        SegmentTree latest(bound);
        for (int value = 1; value <= bound; ++value) latest.set_value(value, INT_MAX);
        for (int power : candidates) latest.set_value(power, 0);
        vector<int> previous_obstacle(bound + 1, 0);
        vector<char> possible(bound + 1, false);

        for (int index = 1; index <= n; ++index) {
            int value = a[index];
            vector<int> factors;
            while (value > 1) {
                int prime = smallest_prime_factor[value];
                int power = 1;
                while (value % prime == 0) {
                    value /= prime;
                    power *= prime;
                    factors.push_back(power);
                }
            }

            // Before index is made available as a factor, it closes every block for
            // which it is an obstacle (i.e. its value is divisible by that power).
            for (int x : factors) {
                if (x > bound) continue;
                if (previous_obstacle[x] + 1 <= index - 1 &&
                    latest.minimum(1, x - 1) > previous_obstacle[x]) {
                    possible[x] = true;
                }
                previous_obstacle[x] = index;
            }
            for (int power : factors) latest.set_value(power, index);
        }

        // The suffix after the final obstacle is another maximal x-free block.
        for (int x : candidates) {
            if (previous_obstacle[x] < n && latest.minimum(1, x - 1) > previous_obstacle[x]) {
                possible[x] = true;
            }
        }

        vector<int> answer;
        for (int x : candidates) {
            if (possible[x]) answer.push_back(x);
        }

        cout << answer.size() << '\n';
        for (int x : answer) cout << x << ' ';
        cout << '\n';
    }
}
