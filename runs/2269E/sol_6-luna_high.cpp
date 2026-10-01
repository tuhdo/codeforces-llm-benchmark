#include <bits/stdc++.h>
using namespace std;

struct PersistentBinaryTrie {
    struct Node {
        int child[2] = {0, 0};
        int count = 0;
    };

    vector<Node> nodes;
    vector<int> roots;

    explicit PersistentBinaryTrie(const vector<int>& values) {
        nodes.reserve((size_t)values.size() * 19);
        nodes.push_back(Node{});
        roots.resize(values.size() + 1);
        for (int i = 0; i < (int)values.size(); ++i) {
            roots[i + 1] = insert(roots[i], values[i]);
        }
    }

    int insert(int previousRoot, int value) {
        int newRoot = (int)nodes.size();
        nodes.push_back(nodes[previousRoot]);
        ++nodes[newRoot].count;

        int oldNode = previousRoot;
        int newNode = newRoot;
        for (int bit = 17; bit >= 0; --bit) {
            int direction = (value >> bit) & 1;
            int oldChild = nodes[oldNode].child[direction];
            int newChild = (int)nodes.size();
            nodes.push_back(nodes[oldChild]);
            ++nodes[newChild].count;
            nodes[newNode].child[direction] = newChild;
            oldNode = oldChild;
            newNode = newChild;
        }
        return newRoot;
    }

    int rangeCount(int rightRoot, int leftRoot, int branch) const {
        return nodes[nodes[rightRoot].child[branch]].count -
               nodes[nodes[leftRoot].child[branch]].count;
    }

    int bestMaskedXor(int value, int leftIndex, int rightIndex, int mask) const {
        // Query values[leftIndex..rightIndex], inclusive.
        int rightRoot = roots[rightIndex + 1];
        int leftRoot = roots[leftIndex];
        int answer = 0;

        for (int bit = 17; bit >= 0; --bit) {
            int valueBit = (value >> bit) & 1;
            int chosen = -1;
            if ((mask >> bit) & 1) {
                int preferred = valueBit ^ 1;
                if (rangeCount(rightRoot, leftRoot, preferred) > 0) {
                    chosen = preferred;
                    answer |= 1 << bit;
                } else {
                    chosen = valueBit;
                }
            } else {
                if (rangeCount(rightRoot, leftRoot, 0) > 0) chosen = 0;
                else chosen = 1;
            }
            rightRoot = nodes[rightRoot].child[chosen];
            leftRoot = nodes[leftRoot].child[chosen];
        }
        return answer;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCases;
    cin >> testCases;
    while (testCases--) {
        int n;
        cin >> n;
        vector<int> a(n);
        for (int& value : a) cin >> value;

        vector<int> prefixXor(n + 1, 0);
        for (int i = 0; i < n; ++i) prefixXor[i + 1] = prefixXor[i] ^ a[i];

        vector<int> previousGreater(n), nextGreaterOrEqual(n);
        vector<int> stack;
        for (int i = 0; i < n; ++i) {
            while (!stack.empty() && a[stack.back()] <= a[i]) stack.pop_back();
            previousGreater[i] = stack.empty() ? -1 : stack.back();
            stack.push_back(i);
        }
        stack.clear();
        for (int i = n - 1; i >= 0; --i) {
            while (!stack.empty() && a[stack.back()] < a[i]) stack.pop_back();
            nextGreaterOrEqual[i] = stack.empty() ? n : stack.back();
            stack.push_back(i);
        }

        PersistentBinaryTrie trie(prefixXor);
        int answer = 0;

        auto crossBest = [&](int leftA, int rightA, int leftB, int rightB, int mask) {
            if (leftA > rightA || leftB > rightB) return 0;
            int best = 0;
            if (rightA - leftA <= rightB - leftB) {
                for (int i = leftA; i <= rightA; ++i) {
                    best = max(best, trie.bestMaskedXor(prefixXor[i], leftB, rightB, mask));
                }
            } else {
                for (int i = leftB; i <= rightB; ++i) {
                    best = max(best, trie.bestMaskedXor(prefixXor[i], leftA, rightA, mask));
                }
            }
            return best;
        };

        for (int i = 0; i < n; ++i) {
            int leftPrefix = previousGreater[i] + 1;
            int rightPrefix = nextGreaterOrEqual[i];
            int leftStrictEnd = i - 1;
            int rightStrictStart = i + 2;

            // Intervals ending at i, starting at i, or extending on both sides.
            answer = max(answer, crossBest(leftPrefix, leftStrictEnd, i + 1, i + 1, a[i]));
            answer = max(answer, crossBest(i, i, rightStrictStart, rightPrefix, a[i]));
            answer = max(answer, crossBest(leftPrefix, leftStrictEnd, rightStrictStart, rightPrefix, a[i]));
        }

        cout << answer << '\n';
    }
    return 0;
}
