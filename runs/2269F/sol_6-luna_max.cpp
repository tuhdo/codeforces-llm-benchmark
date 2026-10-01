#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    if (!(cin >> t)) return 0;

    while (t--) {
        int n;
        cin >> n;

        vector<int> p(n + 1);
        for (int i = 1; i <= n; ++i) cin >> p[i];

        // The parent of i is its nearest greater element to the right.
        vector<int> parent(n + 1, 0);
        vector<int> st;
        st.reserve(n);
        for (int i = n; i >= 1; --i) {
            while (!st.empty() && p[st.back()] < p[i]) st.pop_back();
            if (!st.empty()) parent[i] = st.back();
            st.push_back(i);
        }

        vector<int> first_child(n + 1, 0);
        vector<int> next_sibling(n + 1, 0);
        vector<int> child_count(n + 1, 0);
        vector<int> roots;
        roots.reserve(n);
        for (int i = 1; i <= n; ++i) {
            if (parent[i] == 0) roots.push_back(i);
            else ++child_count[parent[i]];
        }
        // Insert in descending index order so each linked child list is
        // ordered from left to right (ascending array index).
        for (int i = n; i >= 1; --i) {
            if (parent[i] != 0) {
                next_sibling[i] = first_child[parent[i]];
                first_child[parent[i]] = i;
            }
        }

        // Since every parent has a larger index, ascending order is postorder.
        vector<long long> subtree_size(n + 1, 1);
        for (int i = 1; i <= n; ++i) {
            if (parent[i] != 0) subtree_size[parent[i]] += subtree_size[i];
        }

        // Mark the leftmost-child chain of every tree.
        vector<char> on_spine(n + 1, false);
        for (int root : roots) {
            int v = root;
            while (true) {
                on_spine[v] = true;
                if (first_child[v] == 0) break;
                v = first_child[v];
            }
        }

        // For spine nodes, distance_sum[v] is the sum of distances to v from
        // all nodes in its subtree, including v itself (whose distance is 0).
        vector<long long> distance_sum(n + 1, 0);
        long long upward_pair_sum = 0;

        for (int v = 1; v <= n; ++v) {
            long long local = 0;

            if (on_spine[v]) {
                if (first_child[v] != 0) {
                    int first = first_child[v];
                    long long first_size = subtree_size[first];
                    long long side_size = subtree_size[v] - 1 - first_size;

                    distance_sum[v] = distance_sum[first] + first_size
                                    + 2 * side_size - (child_count[v] - 1);

                    // Pairs from the first child subtree to later child
                    // subtrees use the route through v and one left move.
                    local = distance_sum[v]
                          + side_size * (distance_sum[first] + 2 * first_size);

                    // Among later child subtrees, a child root reaches any
                    // later target in two steps; its other nodes need three.
                    long long prior_weight = 0;
                    for (int c = next_sibling[first]; c != 0; c = next_sibling[c]) {
                        local += prior_weight * subtree_size[c];
                        prior_weight += 3 * subtree_size[c] - 1;
                    }
                }
            } else {
                long long descendants = subtree_size[v] - 1;
                long long degree = child_count[v];
                long long distance_two = 0;

                if (first_child[v] != 0) {
                    // Later child subtrees can use the first child as a
                    // two-step route to v. In the first subtree, only direct
                    // children of its root reach v in two right moves.
                    int first = first_child[v];
                    long long side_size = subtree_size[v] - 1 - subtree_size[first];
                    distance_two = side_size - (degree - 1) + child_count[first];
                }

                // An earlier sibling of this subtree gives every pair whose
                // LCA is v a route of length three. Count the exact 1- and
                // 2-step pairs, then assign length three to the rest.
                local = 3 * descendants - 2 * degree - distance_two;

                long long prior_weight = 0;
                for (int c = first_child[v]; c != 0; c = next_sibling[c]) {
                    local += prior_weight * subtree_size[c];
                    prior_weight += 3 * subtree_size[c] - 1;
                }
            }

            upward_pair_sum += local;
        }

        long long left_pair_sum = 1LL * n * (n - 1) / 2;
        cout << left_pair_sum + upward_pair_sum << '\n';
    }

    return 0;
}
