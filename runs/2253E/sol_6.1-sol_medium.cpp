#include <bits/stdc++.h>
using namespace std;

constexpr int MOD = 998244353;
constexpr int ROOT = 3;

int mod_power(int a, int e) {
    int result = 1;
    while (e) {
        if (e & 1) result = (long long)result * a % MOD;
        a = (long long)a * a % MOD;
        e >>= 1;
    }
    return result;
}

void ntt(vector<int>& a, bool inverse) {
    int n = (int)a.size();
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }
    for (int len = 2; len <= n; len <<= 1) {
        int step = mod_power(ROOT, (MOD - 1) / len);
        if (inverse) step = mod_power(step, MOD - 2);
        for (int start = 0; start < n; start += len) {
            int w = 1;
            for (int j = 0; j < len / 2; ++j) {
                int u = a[start + j];
                int v = (long long)a[start + j + len / 2] * w % MOD;
                int sum = u + v;
                if (sum >= MOD) sum -= MOD;
                int difference = u - v;
                if (difference < 0) difference += MOD;
                a[start + j] = sum;
                a[start + j + len / 2] = difference;
                w = (long long)w * step % MOD;
            }
        }
    }
    if (inverse) {
        int inv_n = mod_power(n, MOD - 2);
        for (int& x : a) x = (long long)x * inv_n % MOD;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        vector<int> head(n, -1), to(2 * (n - 1)), next(2 * (n - 1));
        int edges = 0;
        auto add_edge = [&](int u, int v) {
            to[edges] = v;
            next[edges] = head[u];
            head[u] = edges++;
        };
        for (int i = 1; i < n; ++i) {
            int u, v;
            cin >> u >> v;
            --u;
            --v;
            add_edge(u, v);
            add_edge(v, u);
        }

        vector<int> parent(n), depth(n), order;
        order.reserve(n);
        auto farthest = [&](int root) {
            order.clear();
            order.push_back(root);
            parent[root] = -1;
            depth[root] = 0;
            for (int i = 0; i < (int)order.size(); ++i) {
                int u = order[i];
                for (int e = head[u]; e != -1; e = next[e]) {
                    int v = to[e];
                    if (v == parent[u]) continue;
                    parent[v] = u;
                    depth[v] = depth[u] + 1;
                    order.push_back(v);
                }
            }
            return order.back();
        };
        int endpoint = farthest(0);
        int other = farthest(endpoint);
        int h = depth[other] / 2;
        int right_root = other;
        for (int i = 0; i < h; ++i) right_root = parent[right_root];
        int left_root = parent[right_root];

        // Removing the central edge gives two rooted trees of height h.
        vector<unsigned char> side(n);
        order.clear();
        order.push_back(left_root);
        order.push_back(right_root);
        parent[left_root] = right_root;
        parent[right_root] = left_root;
        depth[left_root] = depth[right_root] = 0;
        side[left_root] = 0;
        side[right_root] = 1;
        for (int i = 0; i < (int)order.size(); ++i) {
            int u = order[i];
            for (int e = head[u]; e != -1; e = next[e]) {
                int v = to[e];
                if (v == parent[u]) continue;
                parent[v] = u;
                depth[v] = depth[u] + 1;
                side[v] = side[u];
                order.push_back(v);
            }
        }

        vector<int> possible[2];
        possible[0].assign(h + 1, 0);
        possible[1].assign(h + 1, 0);
        possible[0][h] = possible[1][h] = 1;
        vector<unsigned char> active_children(n, 0);
        for (int i = n - 1; i >= 0; --i) {
            int u = order[i];
            // Two active child subtrees make u an LCA of deepest endpoints.
            if (active_children[u] >= 2) possible[side[u]][depth[u]] = 1;
            if ((depth[u] == h || active_children[u]) && depth[u] != 0) {
                int p = parent[u];
                if (active_children[p] < 2) ++active_children[p];
            }
        }

        int minimum[2] = {0, 0};
        for (int s = 0; s < 2; ++s) {
            while (!possible[s][minimum[s]]) ++minimum[s];
        }
        int offset = minimum[0] + minimum[1] + 1;
        vector<int> a(possible[0].begin() + minimum[0], possible[0].end());
        vector<int> b(possible[1].begin() + minimum[1], possible[1].end());
        vector<int> answer;
        if (a.size() == 1 || b.size() == 1) {
            const vector<int>& values = a.size() == 1 ? b : a;
            for (int i = 0; i < (int)values.size(); ++i) {
                if (values[i]) answer.push_back(offset + i);
            }
        } else {
            int result_size = (int)a.size() + (int)b.size() - 1;
            int size = 1;
            while (size < result_size) size <<= 1;
            bool equal = a == b;
            a.resize(size);
            ntt(a, false);
            if (equal) {
                for (int& x : a) x = (long long)x * x % MOD;
            } else {
                b.resize(size);
                ntt(b, false);
                for (int i = 0; i < size; ++i) a[i] = (long long)a[i] * b[i] % MOD;
            }
            ntt(a, true);
            // Each coefficient is at most h+1, so no positive count is 0 modulo MOD.
            for (int i = 0; i < result_size; ++i) {
                if (a[i]) answer.push_back(offset + i);
            }
        }
        cout << answer.size() << '\n';
        for (int i = 0; i < (int)answer.size(); ++i) {
            if (i) cout << ' ';
            cout << answer[i];
        }
        cout << '\n';
    }
}
