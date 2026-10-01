#include <bits/stdc++.h>
using namespace std;

static constexpr int MAX_BIT = 17;

struct PersistentTrie {
    struct Node {
        int child[2] = {0, 0};
        int count = 0;
    };

    vector<Node> nodes;
    vector<int> roots;

    explicit PersistentTrie(const vector<int>& values) {
        nodes.reserve(1 + (values.size() + 1) * (MAX_BIT + 1));
        nodes.push_back(Node{});
        roots.reserve(values.size() + 1);
        roots.push_back(0);
        for (int x : values) roots.push_back(insert(roots.back(), x, MAX_BIT));
    }

    int insert(int old, int value, int bit) {
        int now = (int)nodes.size();
        nodes.push_back(nodes[old]);
        ++nodes[now].count;
        if (bit >= 0) {
            int b = (value >> bit) & 1;
            nodes[now].child[b] = insert(nodes[old].child[b], value, bit - 1);
        }
        return now;
    }

    int countIn(int rightRoot, int leftRoot) const {
        return nodes[rightRoot].count - nodes[leftRoot].count;
    }

    // Maximum of (x xor y) & mask for y in the inclusive prefix-index range.
    int query(int x, int mask, int left, int right) const {
        if (left > right) return 0;
        return queryNodes(roots[right + 1], roots[left], MAX_BIT, x, mask);
    }

    int queryNodes(int rightNode, int leftNode, int bit, int x, int mask) const {
        if (bit < 0 || mask == 0) return 0;
        if (((mask >> bit) & 1) == 0) {
            return max(queryNodes(nodes[rightNode].child[0], nodes[leftNode].child[0],
                                  bit - 1, x, mask),
                       queryNodes(nodes[rightNode].child[1], nodes[leftNode].child[1],
                                  bit - 1, x, mask));
        }

        int wanted = ((x >> bit) & 1) ^ 1;
        int rightWanted = nodes[rightNode].child[wanted];
        int leftWanted = nodes[leftNode].child[wanted];
        if (countIn(rightWanted, leftWanted) > 0) {
            return (1 << bit) + queryNodes(rightWanted, leftWanted, bit - 1, x, mask);
        }
        int other = wanted ^ 1;
        return queryNodes(nodes[rightNode].child[other], nodes[leftNode].child[other],
                          bit - 1, x, mask);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        vector<int> a(n), pref(n + 1, 0);
        for (int i = 0; i < n; ++i) {
            cin >> a[i];
            pref[i + 1] = pref[i] ^ a[i];
        }

        vector<int> leftChild(n, -1), rightChild(n, -1), parent(n, -1), st;
        st.reserve(n);
        for (int i = 0; i < n; ++i) {
            int last = -1;
            while (!st.empty() && a[st.back()] <= a[i]) {
                last = st.back();
                st.pop_back();
            }
            if (!st.empty()) {
                rightChild[st.back()] = i;
                parent[i] = st.back();
            }
            if (last != -1) {
                leftChild[i] = last;
                parent[last] = i;
            }
            st.push_back(i);
        }

        int root = 0;
        while (parent[root] != -1) root = parent[root];

        vector<int> order;
        order.reserve(n);
        st.clear();
        st.push_back(root);
        while (!st.empty()) {
            int v = st.back();
            st.pop_back();
            order.push_back(v);
            if (leftChild[v] != -1) st.push_back(leftChild[v]);
            if (rightChild[v] != -1) st.push_back(rightChild[v]);
        }

        vector<int> subtreeL(n), subtreeR(n);
        for (int z = n - 1; z >= 0; --z) {
            int v = order[z];
            subtreeL[v] = subtreeR[v] = v;
            if (leftChild[v] != -1) {
                subtreeL[v] = min(subtreeL[v], subtreeL[leftChild[v]]);
                subtreeR[v] = max(subtreeR[v], subtreeR[leftChild[v]]);
            }
            if (rightChild[v] != -1) {
                subtreeL[v] = min(subtreeL[v], subtreeL[rightChild[v]]);
                subtreeR[v] = max(subtreeR[v], subtreeR[rightChild[v]]);
            }
        }

        PersistentTrie trie(pref);
        int answer = 0;

        auto bestBetween = [&](int l1, int r1, int l2, int r2, int mask) {
            if (l1 > r1 || l2 > r2) return 0;
            int len1 = r1 - l1 + 1;
            int len2 = r2 - l2 + 1;
            int best = 0;
            if (len1 <= len2) {
                for (int i = l1; i <= r1; ++i)
                    best = max(best, trie.query(pref[i], mask, l2, r2));
            } else {
                for (int i = l2; i <= r2; ++i)
                    best = max(best, trie.query(pref[i], mask, l1, r1));
            }
            return best;
        };

        for (int v = 0; v < n; ++v) {
            int p = v;
            int L = subtreeL[v], R = subtreeR[v];
            // For [l, r], the prefix indices are l and r + 1.
            // Split around the invalid length-one interval [p, p].
            answer = max(answer, bestBetween(L, p - 1, p + 1, R + 1, a[v]));
            answer = max(answer, bestBetween(p, p, p + 2, R + 1, a[v]));
        }

        cout << answer << '\n';
    }
    return 0;
}
