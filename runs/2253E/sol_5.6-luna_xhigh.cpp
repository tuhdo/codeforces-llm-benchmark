#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

constexpr int MOD = 998244353;
constexpr int ROOT = 3;

int mod_pow(int a, int e) {
    long long result = 1;
    long long base = a;
    while (e > 0) {
        if (e & 1) result = result * base % MOD;
        base = base * base % MOD;
        e >>= 1;
    }
    return static_cast<int>(result);
}

void ntt(vector<int>& a, bool invert) {
    const int n = static_cast<int>(a.size());

    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }

    for (int len = 2; len <= n; len <<= 1) {
        int wlen = mod_pow(ROOT, (MOD - 1) / len);
        if (invert) wlen = mod_pow(wlen, MOD - 2);

        for (int start = 0; start < n; start += len) {
            long long w = 1;
            const int half = len >> 1;
            for (int j = 0; j < half; ++j) {
                const int u = a[start + j];
                const int v = static_cast<int>(a[start + j + half] * w % MOD);
                int x = u + v;
                if (x >= MOD) x -= MOD;
                int y = u - v;
                if (y < 0) y += MOD;
                a[start + j] = x;
                a[start + j + half] = y;
                w = w * wlen % MOD;
            }
        }
    }

    if (invert) {
        const int inv_n = mod_pow(n, MOD - 2);
        for (int& x : a) x = static_cast<int>(1LL * x * inv_n % MOD);
    }
}

struct FastInput {
    static constexpr size_t BUFFER_SIZE = 1 << 20;
    char buffer[BUFFER_SIZE];
    size_t position = 0;
    size_t size = 0;

    char get_char() {
        if (position == size) {
            size = fread(buffer, 1, BUFFER_SIZE, stdin);
            position = 0;
            if (size == 0) return 0;
        }
        return buffer[position++];
    }

    int next_int() {
        char c = get_char();
        while (c <= ' ' && c != 0) c = get_char();
        int value = 0;
        while (c > ' ') {
            value = value * 10 + (c - '0');
            c = get_char();
        }
        return value;
    }
};

struct FastOutput {
    string data;

    void put_int(int value) {
        char digits[12];
        int length = 0;
        do {
            digits[length++] = static_cast<char>('0' + value % 10);
            value /= 10;
        } while (value > 0);
        while (length > 0) data.push_back(digits[--length]);
    }

    void space() { data.push_back(' '); }
    void newline() { data.push_back('\n'); }
    void flush() { fwrite(data.data(), 1, data.size(), stdout); }
};

int analyze_side(int root,
                 int blocked,
                 const vector<int>& head,
                 const vector<int>& to,
                 const vector<int>& next_edge,
                 vector<int>& parent,
                 vector<int>& depth,
                 vector<int>& branch_count,
                 vector<int>& stack,
                 vector<int>& order,
                 vector<char>& possible,
                 int height) {
    stack.clear();
    order.clear();
    stack.push_back(root);
    parent[root] = 0;
    depth[root] = 0;
    branch_count[root] = 0;
    int max_depth = 0;

    while (!stack.empty()) {
        const int v = stack.back();
        stack.pop_back();
        order.push_back(v);
        max_depth = max(max_depth, depth[v]);
        branch_count[v] = 0;

        for (int edge = head[v]; edge != -1; edge = next_edge[edge]) {
            const int u = to[edge];
            if (u == parent[v] || (v == root && u == blocked)) continue;
            parent[u] = v;
            depth[u] = depth[v] + 1;
            stack.push_back(u);
        }
    }

    for (int i = static_cast<int>(order.size()) - 1; i >= 0; --i) {
        const int v = order[i];
        const bool has_deep_vertex = depth[v] == max_depth || branch_count[v] != 0;

        if (branch_count[v] >= 2) possible[depth[v]] = 1;

        if (v != root && has_deep_vertex) {
            const int p = parent[v];
            branch_count[p] = min(2, branch_count[p] + 1);
        }
    }

    possible[height] = 1;
    return max_depth;
}

vector<int> collect_values(const vector<char>& possible) {
    vector<int> values;
    for (int i = 0; i < static_cast<int>(possible.size()); ++i) {
        if (possible[i]) values.push_back(i);
    }
    return values;
}

int main() {
    FastInput input;
    FastOutput output;

    const int test_count = input.next_int();
    for (int test = 0; test < test_count; ++test) {
        const int n = input.next_int();
        vector<int> head(n + 1, -1);
        vector<int> to(2 * max(0, n - 1));
        vector<int> next_edge(2 * max(0, n - 1));
        int edge_count = 0;

        auto add_edge = [&](int u, int v) {
            to[edge_count] = v;
            next_edge[edge_count] = head[u];
            head[u] = edge_count++;
        };

        for (int i = 0; i < n - 1; ++i) {
            const int u = input.next_int();
            const int v = input.next_int();
            add_edge(u, v);
            add_edge(v, u);
        }

        vector<int> parent(n + 1, 0);
        vector<int> distance(n + 1, -1);
        vector<int> stack;
        vector<int> order;
        stack.reserve(n);
        order.reserve(n);

        auto farthest_from = [&](int start) {
            stack.clear();
            stack.push_back(start);
            distance[start] = 0;
            parent[start] = 0;
            int farthest = start;

            while (!stack.empty()) {
                const int v = stack.back();
                stack.pop_back();
                if (distance[v] > distance[farthest]) farthest = v;

                for (int edge = head[v]; edge != -1; edge = next_edge[edge]) {
                    const int u = to[edge];
                    if (u == parent[v]) continue;
                    parent[u] = v;
                    distance[u] = distance[v] + 1;
                    stack.push_back(u);
                }
            }

            return farthest;
        };

        const int endpoint_a = farthest_from(1);
        const int endpoint_b = farthest_from(endpoint_a);
        const int diameter = distance[endpoint_b];
        const int height = diameter / 2;

        vector<int> path;
        for (int v = endpoint_b; v != 0; v = parent[v]) path.push_back(v);
        reverse(path.begin(), path.end());

        const int center_left = path[height];
        const int center_right = path[height + 1];

        vector<int> depth(n + 1, 0);
        vector<int> branch_count(n + 1, 0);
        vector<char> possible_left(height + 1, 0);
        vector<char> possible_right(height + 1, 0);

        analyze_side(center_left, center_right, head, to, next_edge,
                     parent, depth, branch_count, stack, order,
                     possible_left, height);
        analyze_side(center_right, center_left, head, to, next_edge,
                     parent, depth, branch_count, stack, order,
                     possible_right, height);

        const vector<int> left_values = collect_values(possible_left);
        const vector<int> right_values = collect_values(possible_right);
        const int max_sum = 2 * height;
        vector<char> sums(max_sum + 1, 0);

        if (1LL * left_values.size() * right_values.size() <= 4000000LL) {
            for (int x : left_values) {
                for (int y : right_values) sums[x + y] = 1;
            }
        } else {
            int transform_size = 1;
            while (transform_size <= max_sum) transform_size <<= 1;

            vector<int> left_polynomial(transform_size, 0);
            vector<int> right_polynomial(transform_size, 0);
            for (int x : left_values) left_polynomial[x] = 1;
            for (int y : right_values) right_polynomial[y] = 1;

            ntt(left_polynomial, false);
            ntt(right_polynomial, false);
            for (int i = 0; i < transform_size; ++i) {
                left_polynomial[i] = static_cast<int>(
                    1LL * left_polynomial[i] * right_polynomial[i] % MOD);
            }
            ntt(left_polynomial, true);
            for (int i = 0; i <= max_sum; ++i) {
                if (left_polynomial[i] != 0) sums[i] = 1;
            }
        }

        int answer_count = 0;
        for (char present : sums) answer_count += present != 0;
        output.put_int(answer_count);
        for (int sum = 0; sum <= max_sum; ++sum) {
            if (sums[sum]) {
                output.space();
                output.put_int(sum + 1);
            }
        }
        output.newline();
    }

    output.flush();
    return 0;
}
