#include <bits/stdc++.h>
using namespace std;

constexpr int MOD = 998244353;
constexpr int PRIMITIVE_ROOT = 3;

int modPower(int base, int exponent) {
    int result = 1;
    while (exponent > 0) {
        if (exponent & 1) result = 1LL * result * base % MOD;
        base = 1LL * base * base % MOD;
        exponent >>= 1;
    }
    return result;
}

void ntt(vector<int>& a, const vector<int>& roots) {
    const int n = static_cast<int>(a.size());
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }

    for (int half = 1; half < n; half <<= 1) {
        for (int start = 0; start < n; start += 2 * half) {
            for (int j = 0; j < half; ++j) {
                int u = a[start + j];
                int v = 1LL * a[start + j + half] * roots[half + j] % MOD;
                int sum = u + v;
                if (sum >= MOD) sum -= MOD;
                int difference = u - v;
                if (difference < 0) difference += MOD;
                a[start + j] = sum;
                a[start + j + half] = difference;
            }
        }
    }
}

vector<int> convolution(vector<int> a, vector<int> b) {
    // Trimming makes the sole coefficient of a singleton equal to one.
    if (a.size() == 1) return b;
    if (b.size() == 1) return a;

    int resultSize = static_cast<int>(a.size() + b.size() - 1);
    int size = 1;
    while (size < resultSize) size <<= 1;
    a.resize(size);
    b.resize(size);

    vector<int> roots(size);
    for (int half = 1; half < size; half <<= 1) {
        int step = modPower(PRIMITIVE_ROOT, (MOD - 1) / (2 * half));
        roots[half] = 1;
        for (int j = 1; j < half; ++j) {
            roots[half + j] = 1LL * roots[half + j - 1] * step % MOD;
        }
    }

    ntt(a, roots);
    ntt(b, roots);
    for (int i = 0; i < size; ++i) a[i] = 1LL * a[i] * b[i] % MOD;

    // An inverse transform is a forward transform with reversed frequencies.
    reverse(a.begin() + 1, a.end());
    ntt(a, roots);
    int inverseSize = modPower(size, MOD - 2);
    for (int& value : a) value = 1LL * value * inverseSize % MOD;
    a.resize(resultSize);
    return a;
}

struct Tree {
    vector<int> head, to, next;

    explicit Tree(int n) : head(n, -1) {
        to.reserve(2 * (n - 1));
        next.reserve(2 * (n - 1));
    }

    void addEdge(int u, int v) {
        next.push_back(head[u]);
        to.push_back(v);
        head[u] = static_cast<int>(to.size()) - 1;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        Tree tree(n);
        for (int i = 0; i < n - 1; ++i) {
            int u, v;
            cin >> u >> v;
            --u;
            --v;
            tree.addEdge(u, v);
            tree.addEdge(v, u);
        }

        vector<int> parent(n), depth(n), order;
        order.reserve(n);
        auto farthest = [&](int root) {
            order.clear();
            order.push_back(root);
            parent[root] = -1;
            depth[root] = 0;
            int best = root;
            for (int i = 0; i < static_cast<int>(order.size()); ++i) {
                int v = order[i];
                if (depth[v] > depth[best]) best = v;
                for (int edge = tree.head[v]; edge != -1; edge = tree.next[edge]) {
                    int u = tree.to[edge];
                    if (u == parent[v]) continue;
                    parent[u] = v;
                    depth[u] = depth[v] + 1;
                    order.push_back(u);
                }
            }
            return best;
        };

        int endpoint = farthest(0);
        int opposite = farthest(endpoint);
        int h = depth[opposite] / 2;
        int right = opposite;
        for (int i = 0; i < h; ++i) right = parent[right];
        int left = parent[right];

        // Root both components at the endpoints of the central edge.
        vector<unsigned char> side(n), liveChildren(n, 0);
        order.clear();
        order.push_back(left);
        order.push_back(right);
        parent[left] = right;
        parent[right] = left;
        depth[left] = depth[right] = 0;
        side[left] = 0;
        side[right] = 1;
        for (int i = 0; i < static_cast<int>(order.size()); ++i) {
            int v = order[i];
            for (int edge = tree.head[v]; edge != -1; edge = tree.next[edge]) {
                int u = tree.to[edge];
                if (u == parent[v]) continue;
                parent[u] = v;
                depth[u] = depth[v] + 1;
                side[u] = side[v];
                order.push_back(u);
            }
        }

        vector<int> a(h + 1), b(h + 1);
        a[h] = b[h] = 1;
        for (int i = n - 1; i >= 0; --i) {
            int v = order[i];
            if (liveChildren[v] == 2) {
                (side[v] == 0 ? a : b)[depth[v]] = 1;
            }
            bool reachesDeepest = depth[v] == h || liveChildren[v] > 0;
            if (reachesDeepest && v != left && v != right) {
                unsigned char& count = liveChildren[parent[v]];
                if (count < 2) ++count;
            }
        }

        int firstA = 0, firstB = 0;
        while (a[firstA] == 0) ++firstA;
        while (b[firstB] == 0) ++firstB;
        a.erase(a.begin(), a.begin() + firstA);
        b.erase(b.begin(), b.begin() + firstB);
        vector<int> product = convolution(move(a), move(b));
        int offset = firstA + firstB + 1;

        int count = 0;
        for (int value : product) count += value != 0;
        cout << count << '\n';
        bool first = true;
        for (int i = 0; i < static_cast<int>(product.size()); ++i) {
            if (product[i] == 0) continue;
            if (!first) cout << ' ';
            cout << offset + i;
            first = false;
        }
        cout << '\n';
    }
}
