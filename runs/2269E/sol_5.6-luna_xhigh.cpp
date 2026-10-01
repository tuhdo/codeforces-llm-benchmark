#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;

    constexpr int BITS = 18;
    constexpr int VALUE_LIMIT = 1 << BITS;

    while (T--) {
        int n;
        cin >> n;

        vector<int> a(n);
        for (int &x : a) cin >> x;

        // Build a max Cartesian tree. Equal values stay in their original
        // order; this is enough to cover every subarray by a tree node whose
        // subtree contains the whole subarray.
        vector<int> left_child(n, -1), right_child(n, -1), parent(n, -1);
        vector<int> stack_nodes;
        stack_nodes.reserve(n);

        for (int i = 0; i < n; ++i) {
            int last = -1;
            while (!stack_nodes.empty() && a[stack_nodes.back()] < a[i]) {
                last = stack_nodes.back();
                stack_nodes.pop_back();
            }

            if (!stack_nodes.empty()) {
                right_child[stack_nodes.back()] = i;
                parent[i] = stack_nodes.back();
            }
            if (last != -1) {
                left_child[i] = last;
                parent[last] = i;
            }
            stack_nodes.push_back(i);
        }

        int root = 0;
        while (parent[root] != -1) root = parent[root];

        // [subtree_left[u], subtree_right[u]] is the array interval of u's
        // Cartesian-tree subtree.
        vector<int> subtree_left(n), subtree_right(n), order;
        order.reserve(n);
        vector<int> traversal{root};
        while (!traversal.empty()) {
            int u = traversal.back();
            traversal.pop_back();
            order.push_back(u);
            if (left_child[u] != -1) traversal.push_back(left_child[u]);
            if (right_child[u] != -1) traversal.push_back(right_child[u]);
        }

        for (int u = 0; u < n; ++u) {
            subtree_left[u] = subtree_right[u] = u;
        }
        for (int k = n - 1; k >= 0; --k) {
            int u = order[k];
            if (left_child[u] != -1) {
                int v = left_child[u];
                subtree_left[u] = min(subtree_left[u], subtree_left[v]);
                subtree_right[u] = max(subtree_right[u], subtree_right[v]);
            }
            if (right_child[u] != -1) {
                int v = right_child[u];
                subtree_left[u] = min(subtree_left[u], subtree_left[v]);
                subtree_right[u] = max(subtree_right[u], subtree_right[v]);
            }
        }

        vector<int> prefix_xor(n + 1, 0);
        for (int i = 0; i < n; ++i) prefix_xor[i + 1] = prefix_xor[i] ^ a[i];

        vector<int> projected(n + 1);
        vector<int> head(n + 1), special_head(n + 1);
        vector<int> next_node(n), next_special(n);
        vector<int> last_position(VALUE_LIMIT);

        auto feasible = [&](int required_mask) {
            for (int i = 0; i <= n; ++i) {
                projected[i] = prefix_xor[i] & required_mask;
                head[i] = -1;
                special_head[i] = -1;
            }

            // Put each eligible Cartesian-tree node into the bucket where its
            // opposite prefix range ends. The special bucket removes the
            // one-element interval [i, i] when the right side is smaller.
            for (int i = 0; i < n; ++i) {
                if ((a[i] & required_mask) != required_mask) continue;

                int left_size = i - subtree_left[i] + 1;
                int right_size = subtree_right[i] - i + 1;

                if (left_size <= right_size) {
                    int event = subtree_right[i] + 1;
                    next_node[i] = head[event];
                    head[event] = i;
                } else {
                    int event = i;
                    next_node[i] = head[event];
                    head[event] = i;

                    // For y = i + 1, x = i is the invalid length-one pair.
                    // Answer this query before prefix position i is inserted.
                    if (i > 0 && subtree_left[i] <= i - 1) {
                        int special_event = i - 1;
                        next_special[i] = special_head[special_event];
                        special_head[special_event] = i;
                    }
                }
            }

            fill(last_position.begin(), last_position.end(), -1);

            for (int event = 0; event <= n; ++event) {
                last_position[projected[event]] = event;

                for (int i = special_head[event]; i != -1;
                     i = next_special[i]) {
                    int target = projected[i + 1] ^ required_mask;
                    if (last_position[target] >= subtree_left[i]) return true;
                }

                for (int i = head[event]; i != -1; i = next_node[i]) {
                    if (i - subtree_left[i] + 1 <= subtree_right[i] - i + 1) {
                        // x is the left prefix endpoint, and y lies in
                        // [i + 1, subtree_right[i] + 1].
                        for (int x = subtree_left[i]; x <= i; ++x) {
                            int lower_y = i + 1 + (x == i);
                            if (lower_y > subtree_right[i] + 1) continue;
                            int target = projected[x] ^ required_mask;
                            if (last_position[target] >= lower_y) return true;
                        }
                    } else {
                        // y is the right prefix endpoint. The special y=i+1
                        // case was handled in special_head[i-1].
                        for (int y = i + 2; y <= subtree_right[i] + 1; ++y) {
                            int target = projected[y] ^ required_mask;
                            if (last_position[target] >= subtree_left[i]) {
                                return true;
                            }
                        }
                    }
                }
            }
            return false;
        };

        int answer = 0;
        for (int bit = BITS - 1; bit >= 0; --bit) {
            int candidate = answer | (1 << bit);
            if (feasible(candidate)) answer = candidate;
        }

        cout << answer << '\n';
    }

    return 0;
}
