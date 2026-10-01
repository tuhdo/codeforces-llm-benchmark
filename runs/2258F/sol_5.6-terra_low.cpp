#include <bits/stdc++.h>
using namespace std;

// A randomized treap holding the (sorted) adjacent differences of a convex DP.
struct Node {
    long long value, lazy;
    int id, priority, size;
    Node *left, *right;
    Node(long long value_, int id_, int priority_)
        : value(value_), lazy(0), id(id_), priority(priority_), size(1), left(nullptr), right(nullptr) {}
};

static mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int size_of(Node* node) { return node ? node->size : 0; }

void add_lazy(Node* node, long long delta) {
    if (!node) return;
    node->value += delta;
    node->lazy += delta;
}

void push(Node* node) {
    if (!node || node->lazy == 0) return;
    add_lazy(node->left, node->lazy);
    add_lazy(node->right, node->lazy);
    node->lazy = 0;
}

void pull(Node* node) {
    if (node) node->size = 1 + size_of(node->left) + size_of(node->right);
}

// Splits the first k elements from the remaining elements.
pair<Node*, Node*> split_size(Node* node, int k) {
    if (!node) return {nullptr, nullptr};
    push(node);
    if (size_of(node->left) >= k) {
        auto [a, b] = split_size(node->left, k);
        node->left = b;
        pull(node);
        return {a, node};
    }
    auto [a, b] = split_size(node->right, k - size_of(node->left) - 1);
    node->right = a;
    pull(node);
    return {node, b};
}

// Splits by the lexicographic key (value, id): keys strictly smaller than key go left.
pair<Node*, Node*> split_key(Node* node, pair<long long, int> key) {
    if (!node) return {nullptr, nullptr};
    push(node);
    if (make_pair(node->value, node->id) < key) {
        auto [a, b] = split_key(node->right, key);
        node->right = a;
        pull(node);
        return {node, b};
    }
    auto [a, b] = split_key(node->left, key);
    node->left = b;
    pull(node);
    return {a, node};
}

Node* merge_ordered(Node* a, Node* b) {
    if (!a) return b;
    if (!b) return a;
    if (a->priority > b->priority) {
        push(a);
        a->right = merge_ordered(a->right, b);
        pull(a);
        return a;
    }
    push(b);
    b->left = merge_ordered(a, b->left);
    pull(b);
    return b;
}

// Union of two ordered treaps. All keys are unique because id is unique.
Node* unite(Node* a, Node* b) {
    if (!a) return b;
    if (!b) return a;
    if (a->priority < b->priority) swap(a, b);
    push(a);
    auto [less, greater] = split_key(b, {a->value, a->id});
    a->left = unite(a->left, less);
    a->right = unite(a->right, greater);
    pull(a);
    return a;
}

struct DP {
    long long low = 0;   // smallest achievable subtree sum
    long long base = 0;  // DP value at low
    Node* slopes = nullptr;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        vector<int> a(n);
        for (int& x : a) cin >> x;

        vector<vector<int>> graph(n);
        for (int i = 1; i < n; ++i) {
            int u, v;
            cin >> u >> v;
            --u; --v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }

        vector<int> parent(n, -1), order = {0};
        for (int i = 0; i < (int)order.size(); ++i) {
            int v = order[i];
            for (int to : graph[v]) if (to != parent[v]) {
                parent[to] = v;
                order.push_back(to);
            }
        }

        vector<DP> dp(n);
        int next_id = 0;
        for (int at = n - 1; at >= 0; --at) {
            int v = order[at];
            DP current;
            for (int to : graph[v]) if (parent[to] == v) {
                current.low += dp[to].low;
                current.base += dp[to].base;
                current.slopes = unite(current.slopes, dp[to].slopes);
            }

            if (a[v] == 0) {
                // Min-convolution with the two choices {-1, +1} inserts a zero slope.
                --current.low;
                current.slopes = unite(current.slopes, new Node(0, next_id++, (int)rng()));
            } else {
                current.low += a[v];
            }

            // Add |subtree sum|. Its adjacent differences are -2, possibly 0,
            // then +2, depending on which sums straddle zero.
            current.base += llabs(current.low);
            int m = size_of(current.slopes);
            int negative = 0;
            if (current.low <= -2) {
                negative = min<long long>(m, (-2 - current.low) / 2 + 1);
            }
            long long high = current.low + 2LL * m;
            int middle = (current.low <= -1 && -1 <= high && ((-1 - current.low) % 2 == 0));

            auto [left, rest] = split_size(current.slopes, negative);
            auto [center, right] = split_size(rest, middle);
            add_lazy(left, -2);
            add_lazy(right, 2);
            current.slopes = merge_ordered(merge_ordered(left, center), right);
            dp[v] = current;
        }

        Node* root = dp[0].slopes;
        long long answer = dp[0].base;
        vector<Node*> stack;
        while (root || !stack.empty()) {
            while (root) {
                push(root);
                stack.push_back(root);
                root = root->left;
            }
            root = stack.back();
            stack.pop_back();
            if (root->value < 0) answer += root->value;
            root = root->right;
        }
        cout << answer << '\n';
    }
}
