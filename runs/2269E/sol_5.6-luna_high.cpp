#include <bits/stdc++.h>
using namespace std;

static constexpr int B = 18;
static constexpr int VALUE_LIMIT = 1 << B;

struct PersistentTrie {
    struct Node {
        int child[2]{};
        int count = 0;
    };

    vector<Node> nodes;
    vector<int> roots;

    explicit PersistentTrie(const vector<int>& values) {
        nodes.reserve((values.size() + 1) * (B + 1));
        nodes.push_back(Node{});
        roots.assign(values.size() + 1, 0);
        for (int i = 0; i < (int)values.size(); ++i) {
            roots[i + 1] = insert(roots[i], values[i], B - 1);
        }
    }

    int insert(int previous, int value, int bit) {
        int current = (int)nodes.size();
        nodes.push_back(nodes[previous]);
        ++nodes[current].count;
        if (bit >= 0) {
            int side = (value >> bit) & 1;
            int next = insert(nodes[previous].child[side], value, bit - 1);
            nodes[current].child[side] = next;
        }
        return current;
    }

    int rangeCount(int rightNode, int leftNode) const {
        return nodes[rightNode].count - nodes[leftNode].count;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCount;
    cin >> testCount;
    while (testCount--) {
        int n;
        cin >> n;

        vector<int> a(n + 1);
        for (int i = 1; i <= n; ++i) cin >> a[i];

        vector<int> prefix(n + 1, 0);
        for (int i = 1; i <= n; ++i) prefix[i] = prefix[i - 1] ^ a[i];

        vector<int> leftChild(n + 1, 0), rightChild(n + 1, 0), parent(n + 1, 0);
        vector<int> stack;
        stack.reserve(n);
        for (int i = 1; i <= n; ++i) {
            int last = 0;
            while (!stack.empty() && a[stack.back()] <= a[i]) {
                last = stack.back();
                stack.pop_back();
            }
            if (!stack.empty()) {
                rightChild[stack.back()] = i;
                parent[i] = stack.back();
            }
            if (last != 0) {
                leftChild[i] = last;
                parent[last] = i;
            }
            stack.push_back(i);
        }
        int root = stack.front();

        vector<int> order;
        order.reserve(n);
        order.push_back(root);
        for (int i = 0; i < (int)order.size(); ++i) {
            int u = order[i];
            if (leftChild[u]) order.push_back(leftChild[u]);
            if (rightChild[u]) order.push_back(rightChild[u]);
        }

        vector<int> subtreeLeft(n + 1), subtreeRight(n + 1);
        for (int p = n - 1; p >= 0; --p) {
            int u = order[p];
            subtreeLeft[u] = leftChild[u] ? subtreeLeft[leftChild[u]] : u;
            subtreeRight[u] = rightChild[u] ? subtreeRight[rightChild[u]] : u;
        }

        PersistentTrie trie(prefix);
        int answer = 0;

        auto updateByBruteForce = [&](int loA, int hiA, int loB, int hiB, int mask) {
            if (loA > hiA || loB > hiB) return;
            for (int i = loA; i <= hiA; ++i) {
                for (int j = loB; j <= hiB; ++j) {
                    answer = max(answer, (prefix[i] ^ prefix[j]) & mask);
                }
            }
        };

        auto processSmallSideWithTrie = [&](int loA, int hiA, int loB, int hiB, int mask) {
            if (loA > hiA || loB > hiB || answer >= mask) return;

            int lenA = hiA - loA + 1;
            int lenB = hiB - loB + 1;
            if (1LL * lenA * lenB <= 512) {
                updateByBruteForce(loA, hiA, loB, hiB, mask);
                return;
            }

            int smallLo = loA, smallHi = hiA;
            int queryLo = loB, queryHi = hiB;
            if (lenA > lenB) {
                smallLo = loB;
                smallHi = hiB;
                queryLo = loA;
                queryHi = hiA;
            }

            function<void(int, int, int, int, int)> search =
                [&](int rightNode, int leftNode, int bit, int value, int q) {
                    if (rightNode == leftNode) return;
                    if (bit < 0) {
                        answer = max(answer, value);
                        return;
                    }

                    int lowerPossible = mask & ((1 << (bit + 1)) - 1);
                    if ((value | lowerPossible) <= answer) return;

                    int qBit = (q >> bit) & 1;
                    if ((mask >> bit) & 1) {
                        int preferred = qBit ^ 1;
                        int preferredRight = trie.nodes[rightNode].child[preferred];
                        int preferredLeft = trie.nodes[leftNode].child[preferred];
                        if (trie.rangeCount(preferredRight, preferredLeft) > 0) {
                            search(preferredRight, preferredLeft, bit - 1,
                                   value | (1 << bit), q);
                        } else {
                            int other = qBit;
                            search(trie.nodes[rightNode].child[other],
                                   trie.nodes[leftNode].child[other], bit - 1,
                                   value, q);
                        }
                    } else {
                        int rightZero = trie.nodes[rightNode].child[0];
                        int leftZero = trie.nodes[leftNode].child[0];
                        int rightOne = trie.nodes[rightNode].child[1];
                        int leftOne = trie.nodes[leftNode].child[1];

                        int countZero = trie.rangeCount(rightZero, leftZero);
                        int countOne = trie.rangeCount(rightOne, leftOne);
                        if (countOne >= countZero) {
                            if (countOne) search(rightOne, leftOne, bit - 1, value, q);
                            if (countZero && answer < mask)
                                search(rightZero, leftZero, bit - 1, value, q);
                        } else {
                            if (countZero) search(rightZero, leftZero, bit - 1, value, q);
                            if (countOne && answer < mask)
                                search(rightOne, leftOne, bit - 1, value, q);
                        }
                    }
                };

            int rightRoot = trie.roots[queryHi + 1];
            int leftRoot = trie.roots[queryLo];
            for (int i = smallLo; i <= smallHi && answer < mask; ++i) {
                search(rightRoot, leftRoot, B - 1, 0, prefix[i]);
            }
        };

        // Sparse masks are cheap to answer after grouping by their selected bits.
        vector<vector<int>> sparseGroups(VALUE_LIMIT);
        for (int u : order) {
            int mask = a[u];
            if (mask != 0 && __builtin_popcount((unsigned)mask) <= 3) {
                sparseGroups[mask].push_back(u);
            }
        }

        auto processSparseNode = [&](int u, int mask, const vector<vector<int>>& positions,
                                     const vector<int>& compressedValue) {
            if (answer >= mask) return;
            int pos = u;
            int L = subtreeLeft[u], R = subtreeRight[u];
            int leftLo = L - 1, leftHi = pos - 2;
            int rightLo = pos, rightHi = R;

            auto cross = [&](int loA, int hiA, int loB, int hiB) {
                if (loA > hiA || loB > hiB) return;
                int lenA = hiA - loA + 1;
                int lenB = hiB - loB + 1;
                int smallLo = loA, smallHi = hiA;
                int queryLo = loB, queryHi = hiB;
                if (lenA > lenB) {
                    smallLo = loB;
                    smallHi = hiB;
                    queryLo = loA;
                    queryHi = hiA;
                }
                for (int i = smallLo; i <= smallHi && answer < mask; ++i) {
                    int code = 0;
                    int bitIndex = 0;
                    for (int bit = 0; bit < B; ++bit) {
                        if ((mask >> bit) & 1) {
                            if ((prefix[i] >> bit) & 1) code |= 1 << bitIndex;
                            ++bitIndex;
                        }
                    }
                    for (int other = 0; other < (int)positions.size(); ++other) {
                        const auto& list = positions[other];
                        auto it = lower_bound(list.begin(), list.end(), queryLo);
                        if (it != list.end() && *it <= queryHi) {
                            answer = max(answer, compressedValue[code ^ other]);
                        }
                    }
                }
            };

            cross(leftLo, leftHi, rightLo, rightHi);
            cross(pos - 1, pos - 1, pos + 1, rightHi);
        };

        for (int mask = 1; mask < VALUE_LIMIT; ++mask) {
            if (sparseGroups[mask].empty()) continue;
            int selectedBits = __builtin_popcount((unsigned)mask);
            if (selectedBits > 3) continue;

            vector<int> bits;
            for (int bit = 0; bit < B; ++bit) {
                if ((mask >> bit) & 1) bits.push_back(bit);
            }
            int codeCount = 1 << selectedBits;
            vector<vector<int>> positions(codeCount);
            for (int i = 0; i <= n; ++i) {
                int code = 0;
                for (int j = 0; j < selectedBits; ++j) {
                    if ((prefix[i] >> bits[j]) & 1) code |= 1 << j;
                }
                positions[code].push_back(i);
            }

            vector<int> compressedValue(codeCount, 0);
            for (int code = 0; code < codeCount; ++code) {
                for (int j = 0; j < selectedBits; ++j) {
                    if ((code >> j) & 1) compressedValue[code] |= 1 << bits[j];
                }
            }
            for (int u : sparseGroups[mask]) {
                processSparseNode(u, mask, positions, compressedValue);
            }
        }

        // All remaining masks use the persistent trie. The search is exact, and its
        // upper-bound pruning stops as soon as the node cannot beat the current answer.
        for (int u : order) {
            int mask = a[u];
            if (mask == 0 || __builtin_popcount((unsigned)mask) <= 3 || answer >= mask) continue;

            int pos = u;
            int L = subtreeLeft[u], R = subtreeRight[u];
            processSmallSideWithTrie(L - 1, pos - 2, pos, R, mask);
            processSmallSideWithTrie(pos - 1, pos - 1, pos + 1, R, mask);
        }

        cout << answer << '\n';
    }
    return 0;
}
