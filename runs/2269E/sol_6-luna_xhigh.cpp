#include <bits/stdc++.h>
using namespace std;

struct TrieNode {
    int child[2] = {0, 0};
    int count = 0;
};

class PrefixTrie {
public:
    vector<TrieNode> nodes;
    vector<int> roots;

    explicit PrefixTrie(const vector<int>& values) {
        nodes.reserve(1 + values.size() * 19);
        nodes.emplace_back();
        roots.resize(values.size() + 1);
        for (int i = 0; i < (int)values.size(); ++i) {
            roots[i + 1] = insert(roots[i], values[i]);
        }
    }

    int insert(int oldRoot, int value) {
        int newRoot = clone(oldRoot);
        int oldNode = oldRoot;
        int newNode = newRoot;
        ++nodes[newNode].count;
        for (int bit = 17; bit >= 0; --bit) {
            int side = (value >> bit) & 1;
            int oldChild = nodes[oldNode].child[side];
            int newChild = clone(oldChild);
            nodes[newNode].child[side] = newChild;
            oldNode = oldChild;
            newNode = newChild;
            ++nodes[newNode].count;
        }
        return newRoot;
    }

    int rangeCount(int highRoot, int lowRoot) const {
        return nodes[highRoot].count - nodes[lowRoot].count;
    }

private:
    int clone(int node) {
        nodes.push_back(nodes[node]);
        return (int)nodes.size() - 1;
    }
};

class ProjectedTrie {
public:
    vector<TrieNode> nodes;
    vector<int> roots;
    vector<int> bits;

    ProjectedTrie(const vector<int>& values, int mask) {
        for (int bit = 0; bit < 18; ++bit) {
            if ((mask >> bit) & 1) bits.push_back(bit);
        }
        int k = (int)bits.size();
        nodes.reserve(1 + values.size() * (k + 1));
        nodes.emplace_back();
        roots.resize(values.size() + 1);
        for (int i = 0; i < (int)values.size(); ++i) {
            int compressed = compress(values[i]);
            roots[i + 1] = insert(roots[i], compressed, k);
        }
    }

    int compress(int value) const {
        int result = 0;
        for (int i = 0; i < (int)bits.size(); ++i) {
            if ((value >> bits[i]) & 1) result |= 1 << i;
        }
        return result;
    }

    int insert(int oldRoot, int value, int k) {
        int newRoot = clone(oldRoot);
        int oldNode = oldRoot;
        int newNode = newRoot;
        ++nodes[newNode].count;
        for (int bit = k - 1; bit >= 0; --bit) {
            int side = (value >> bit) & 1;
            int oldChild = nodes[oldNode].child[side];
            int newChild = clone(oldChild);
            nodes[newNode].child[side] = newChild;
            oldNode = oldChild;
            newNode = newChild;
            ++nodes[newNode].count;
        }
        return newRoot;
    }

    int query(int value, int lo, int hi) const {
        int highNode = roots[hi + 1];
        int lowNode = roots[lo];
        int result = 0;
        for (int bit = (int)bits.size() - 1; bit >= 0; --bit) {
            int wanted = ((value >> bit) & 1) ^ 1;
            int highChild = nodes[highNode].child[wanted];
            int lowChild = nodes[lowNode].child[wanted];
            if (rangeCount(highChild, lowChild) > 0) {
                result |= 1 << bit;
                highNode = highChild;
                lowNode = lowChild;
            } else {
                int side = wanted ^ 1;
                highNode = nodes[highNode].child[side];
                lowNode = nodes[lowNode].child[side];
            }
        }

        int expanded = 0;
        for (int bit = 0; bit < (int)bits.size(); ++bit) {
            if ((result >> bit) & 1) expanded |= 1 << bits[bit];
        }
        return expanded;
    }

private:
    int clone(int node) {
        nodes.push_back(nodes[node]);
        return (int)nodes.size() - 1;
    }

    int rangeCount(int highRoot, int lowRoot) const {
        return nodes[highRoot].count - nodes[lowRoot].count;
    }
};

class MaskedRangeQuery {
public:
    const PrefixTrie& trie;
    long long& work;
    long long limit;
    bool aborted = false;

    MaskedRangeQuery(const PrefixTrie& trie, long long& work, long long limit)
        : trie(trie), work(work), limit(limit) {}

    int query(int x, int lo, int hi, int mask) {
        if (lo > hi) return -1;
        int highRoot = trie.roots[hi + 1];
        int lowRoot = trie.roots[lo];
        return dfs(highRoot, lowRoot, 17, x, mask, 0);
    }

private:
    int dfs(int highNode, int lowNode, int bit, int x, int mask, int score) {
        int total = trie.rangeCount(highNode, lowNode);
        if (total == 0) return -1;
        if (++work > limit) {
            aborted = true;
            return 0;
        }
        if (bit < 0) return score;

        // No remaining output bit can change the answer.
        if ((mask & ((1 << (bit + 1)) - 1)) == 0) return score;

        int xBit = (x >> bit) & 1;
        if ((mask >> bit) & 1) {
            int wanted = xBit ^ 1;
            int highChild = trie.nodes[highNode].child[wanted];
            int lowChild = trie.nodes[lowNode].child[wanted];
            if (trie.rangeCount(highChild, lowChild) > 0) {
                return dfs(highChild, lowChild, bit - 1, x, mask, score | (1 << bit));
            }
            int side = wanted ^ 1;
            return dfs(trie.nodes[highNode].child[side], trie.nodes[lowNode].child[side],
                       bit - 1, x, mask, score);
        }

        int h0 = trie.nodes[highNode].child[0];
        int l0 = trie.nodes[lowNode].child[0];
        int h1 = trie.nodes[highNode].child[1];
        int l1 = trie.nodes[lowNode].child[1];
        int c0 = trie.rangeCount(h0, l0);
        int c1 = trie.rangeCount(h1, l1);
        if (c0 == 0) return dfs(h1, l1, bit - 1, x, mask, score);
        if (c1 == 0) return dfs(h0, l0, bit - 1, x, mask, score);

        int firstH = c0 >= c1 ? h0 : h1;
        int firstL = c0 >= c1 ? l0 : l1;
        int secondH = c0 >= c1 ? h1 : h0;
        int secondL = c0 >= c1 ? l1 : l0;
        int first = dfs(firstH, firstL, bit - 1, x, mask, score);
        if (aborted) return 0;
        int upperBound = score | (mask & ((1 << bit) - 1));
        if (first == upperBound) return first;
        int second = dfs(secondH, secondL, bit - 1, x, mask, score);
        if (aborted) return 0;
        return max(first, second);
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

        vector<int> prefix(n + 1, 0);
        for (int i = 0; i < n; ++i) prefix[i + 1] = prefix[i] ^ a[i];

        vector<int> previousGreater(n), nextGreaterOrEqual(n);
        vector<int> stack;
        stack.reserve(n);
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

        vector<int> order(n);
        iota(order.begin(), order.end(), 0);
        sort(order.begin(), order.end(), [&](int x, int y) { return a[x] < a[y]; });

        PrefixTrie trie(prefix);
        int answer = 0;

        auto forEachQuery = [&](int begin, int end, auto&& callback) {
            for (int pos = begin; pos < end; ++pos) {
                int i = order[pos];
                int leftCount = i - previousGreater[i];
                int rightCount = nextGreaterOrEqual[i] - i;

                if (leftCount <= rightCount) {
                    for (int p = previousGreater[i] + 1; p <= i; ++p) {
                        int lo = i + 1;
                        int hi = nextGreaterOrEqual[i];
                        if (p == i) ++lo; // Exclude the one-element segment [i, i+1).
                        if (lo <= hi) callback(prefix[p], lo, hi);
                    }
                } else {
                    for (int q = i + 1; q <= nextGreaterOrEqual[i]; ++q) {
                        int lo = previousGreater[i] + 1;
                        int hi = i;
                        if (q == i + 1) --hi; // Exclude the one-element segment [i, i+1).
                        if (lo <= hi) callback(prefix[q], lo, hi);
                    }
                }
            }
        };

        for (int begin = 0; begin < n;) {
            int end = begin + 1;
            int mask = a[order[begin]];
            while (end < n && a[order[end]] == mask) ++end;
            if (mask == 0) {
                begin = end;
                continue;
            }

            int bitCount = __builtin_popcount((unsigned)mask);
            long long queryCount = 0;
            for (int pos = begin; pos < end; ++pos) {
                int i = order[pos];
                int leftCount = i - previousGreater[i];
                int rightCount = nextGreaterOrEqual[i] - i;
                if (leftCount <= rightCount) {
                    queryCount += leftCount - (rightCount == 1 ? 1 : 0);
                } else {
                    queryCount += rightCount - (leftCount == 1 ? 1 : 0);
                }
            }
            if (queryCount == 0) {
                begin = end;
                continue;
            }

            long long work = 0;
            long long directLimit = (long long)(n + 1) * (bitCount + 1) + queryCount * bitCount;
            MaskedRangeQuery maskedQuery(trie, work, directLimit);
            forEachQuery(begin, end, [&](int x, int lo, int hi) {
                if (maskedQuery.aborted) return;
                int candidate = maskedQuery.query(x, lo, hi, mask);
                if (!maskedQuery.aborted) answer = max(answer, candidate);
            });

            if (maskedQuery.aborted) {
                ProjectedTrie projected(prefix, mask);
                forEachQuery(begin, end, [&](int x, int lo, int hi) {
                    answer = max(answer, projected.query(projected.compress(x), lo, hi));
                });
            }
            begin = end;
        }

        cout << answer << '\n';
    }
    return 0;
}
