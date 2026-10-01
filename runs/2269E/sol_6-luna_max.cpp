#include <bits/stdc++.h>
using namespace std;

struct TrieNode {
    int child[2] = {0, 0};
    int count = 0;
};

struct PairNode {
    int high;
    int low;
};

struct Query {
    int mask;
    int leftLo, leftHi;
    int rightLo, rightHi;
};

static vector<TrieNode> baseTrie;
static vector<int> baseRoots;
static vector<PairNode> activeNodes, nextNodes, preferredNodes, fallbackNodes;

static int rangeCount(int highNode, int lowNode) {
    return baseTrie[highNode].count - baseTrie[lowNode].count;
}

// Find max(((x XOR y) AND mask)) for y in prefixXor[lo..hi].
// At an unmasked bit, both trie branches remain viable.  Keeping all of
// those branches lets the next masked bit be optimized globally.
static int maxMaskedXorInRange(int x, int lo, int hi, int mask) {
    activeNodes.clear();
    nextNodes.clear();
    preferredNodes.clear();
    fallbackNodes.clear();

    activeNodes.push_back({baseRoots[hi + 1], baseRoots[lo]});
    int answer = 0;
    int lowest = __builtin_ctz((unsigned)mask);

    for (int bit = 17; bit >= lowest; --bit) {
        if ((mask >> bit) & 1) {
            int xBit = (x >> bit) & 1;
            int preferredBit = xBit ^ 1;
            bool canSet = false;

            for (const PairNode &p : activeNodes) {
                int h = baseTrie[p.high].child[preferredBit];
                int l = baseTrie[p.low].child[preferredBit];
                if (rangeCount(h, l) > 0) {
                    canSet = true;
                    break;
                }
            }

            int chosenBit = canSet ? preferredBit : xBit;
            if (canSet) answer |= (1 << bit);

            nextNodes.clear();
            for (const PairNode &p : activeNodes) {
                int h = baseTrie[p.high].child[chosenBit];
                int l = baseTrie[p.low].child[chosenBit];
                if (rangeCount(h, l) > 0) nextNodes.push_back({h, l});
            }
            activeNodes.swap(nextNodes);
        } else {
            nextNodes.clear();
            for (const PairNode &p : activeNodes) {
                for (int b = 0; b < 2; ++b) {
                    int h = baseTrie[p.high].child[b];
                    int l = baseTrie[p.low].child[b];
                    if (rangeCount(h, l) > 0) nextNodes.push_back({h, l});
                }
            }
            activeNodes.swap(nextNodes);
        }
    }
    return answer;
}

static int appendTrieNode(vector<TrieNode> &trie, int from) {
    trie.push_back(trie[from]);
    return (int)trie.size() - 1;
}

static int compressBits(int value, const vector<int> &bits) {
    int result = 0;
    for (int bit : bits) result = (result << 1) | ((value >> bit) & 1);
    return result;
}

struct ProjectedTrie {
    vector<TrieNode> trie;
    vector<int> roots;
    vector<int> bits;
    int baseIndex = 0;

    ProjectedTrie(const vector<int> &prefixXor, int lo, int hi, int mask)
        : baseIndex(lo) {
        for (int b = 17; b >= 0; --b) {
            if ((mask >> b) & 1) bits.push_back(b);
        }

        int width = (int)bits.size();
        trie.reserve(1 + (hi - lo + 1) * (width + 1));
        trie.push_back(TrieNode{});
        roots.reserve(hi - lo + 2);
        roots.push_back(0);

        for (int pos = lo; pos <= hi; ++pos) {
            int key = compressBits(prefixXor[pos], bits);
            int oldRoot = roots.back();
            int newRoot = appendTrieNode(trie, oldRoot);
            ++trie[newRoot].count;
            int oldNode = oldRoot;
            int newNode = newRoot;

            for (int b = width - 1; b >= 0; --b) {
                int branch = (key >> b) & 1;
                int oldChild = trie[oldNode].child[branch];
                int newChild = appendTrieNode(trie, oldChild);
                ++trie[newChild].count;
                trie[newNode].child[branch] = newChild;
                oldNode = oldChild;
                newNode = newChild;
            }
            roots.push_back(newRoot);
        }
    }

    int query(int x, int lo, int hi) const {
        int key = compressBits(x, bits);
        int high = roots[hi - baseIndex + 1];
        int low = roots[lo - baseIndex];
        int result = 0;
        int width = (int)bits.size();

        for (int b = width - 1; b >= 0; --b) {
            int xBit = (key >> b) & 1;
            int preferred = xBit ^ 1;
            int preferredHigh = trie[high].child[preferred];
            int preferredLow = trie[low].child[preferred];
            int chosen;
            if (trie[preferredHigh].count - trie[preferredLow].count > 0) {
                chosen = preferred;
                result |= (1 << bits[width - 1 - b]);
            } else {
                chosen = xBit;
            }
            high = trie[high].child[chosen];
            low = trie[low].child[chosen];
        }
        return result;
    }
};

static long long directCostPerValue(int mask) {
    int highest = 31 - __builtin_clz((unsigned)mask);
    int lowest = __builtin_ctz((unsigned)mask);
    long long frontier = 1;
    long long cost = 17 - highest; // Prefix values share these higher bits in a valid maximum window.
    for (int bit = highest; bit >= lowest; --bit) {
        cost += frontier;
        if (((mask >> bit) & 1) == 0) frontier <<= 1;
    }
    return cost;
}

static void processQueryDirect(const Query &q, const vector<int> &prefixXor,
                               int &answer) {
    int leftSize = q.leftHi - q.leftLo + 1;
    int rightSize = q.rightHi - q.rightLo + 1;
    if (leftSize <= rightSize) {
        for (int i = q.leftLo; i <= q.leftHi; ++i) {
            answer = max(answer, maxMaskedXorInRange(
                                     prefixXor[i], q.rightLo, q.rightHi, q.mask));
        }
    } else {
        for (int i = q.rightLo; i <= q.rightHi; ++i) {
            answer = max(answer, maxMaskedXorInRange(
                                     prefixXor[i], q.leftLo, q.leftHi, q.mask));
        }
    }
}

static void processQueryProjected(const Query &q, const vector<int> &prefixXor,
                                  const ProjectedTrie &trie, int &answer) {
    int leftSize = q.leftHi - q.leftLo + 1;
    int rightSize = q.rightHi - q.rightLo + 1;
    if (leftSize <= rightSize) {
        for (int i = q.leftLo; i <= q.leftHi; ++i) {
            answer = max(answer, trie.query(prefixXor[i], q.rightLo, q.rightHi));
        }
    } else {
        for (int i = q.rightLo; i <= q.rightHi; ++i) {
            answer = max(answer, trie.query(prefixXor[i], q.leftLo, q.leftHi));
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCases;
    if (!(cin >> testCases)) return 0;
    while (testCases--) {
        int n;
        cin >> n;
        vector<int> a(n);
        for (int &x : a) cin >> x;

        vector<int> prefixXor(n + 1, 0);
        for (int i = 0; i < n; ++i) prefixXor[i + 1] = prefixXor[i] ^ a[i];

        vector<int> previousGreater(n), nextGreaterEqual(n);
        vector<int> st;
        st.reserve(n);
        for (int i = 0; i < n; ++i) {
            while (!st.empty() && a[st.back()] <= a[i]) st.pop_back();
            previousGreater[i] = st.empty() ? -1 : st.back();
            st.push_back(i);
        }
        st.clear();
        for (int i = n - 1; i >= 0; --i) {
            while (!st.empty() && a[st.back()] < a[i]) st.pop_back();
            nextGreaterEqual[i] = st.empty() ? n : st.back();
            st.push_back(i);
        }

        baseTrie.clear();
        baseTrie.reserve(1 + (n + 1) * 19);
        baseTrie.push_back(TrieNode{});
        baseRoots.assign(n + 2, 0);

        for (int i = 0; i <= n; ++i) {
            int oldRoot = baseRoots[i];
            int newRoot = appendTrieNode(baseTrie, oldRoot);
            ++baseTrie[newRoot].count;
            int oldNode = oldRoot;
            int newNode = newRoot;
            for (int bit = 17; bit >= 0; --bit) {
                int branch = (prefixXor[i] >> bit) & 1;
                int oldChild = baseTrie[oldNode].child[branch];
                int newChild = appendTrieNode(baseTrie, oldChild);
                ++baseTrie[newChild].count;
                baseTrie[newNode].child[branch] = newChild;
                oldNode = oldChild;
                newNode = newChild;
            }
            baseRoots[i + 1] = newRoot;
        }

        vector<Query> queries;
        queries.reserve(2 * n);
        for (int i = 0; i < n; ++i) {
            int mask = a[i];
            if (mask == 0) continue;

            // Assign every interval to its rightmost maximum at i.
            // Intervals extending to the left of i:
            int leftLo = previousGreater[i] + 1;
            int leftHi = i - 1;
            int rightLo = i + 1;
            int rightHi = nextGreaterEqual[i];
            if (leftLo <= leftHi && rightLo <= rightHi) {
                queries.push_back({mask, leftLo, leftHi, rightLo, rightHi});
            }

            // Intervals starting at i must extend at least one position right.
            leftLo = i;
            leftHi = i;
            rightLo = i + 2;
            rightHi = nextGreaterEqual[i];
            if (rightLo <= rightHi) {
                queries.push_back({mask, leftLo, leftHi, rightLo, rightHi});
            }
        }

        sort(queries.begin(), queries.end(), [](const Query &x, const Query &y) {
            return x.mask < y.mask;
        });


        activeNodes.reserve(n + 1);
        nextNodes.reserve(n + 1);
        preferredNodes.reserve(n + 1);
        fallbackNodes.reserve(n + 1);

        int answer = 0;
        for (size_t begin = 0; begin < queries.size();) {
            size_t end = begin + 1;
            while (end < queries.size() && queries[end].mask == queries[begin].mask) ++end;
            int mask = queries[begin].mask;
            int lowIndex = n + 1, highIndex = -1;
            long long pairQueries = 0;
            for (size_t q = begin; q < end; ++q) {
                const Query &cur = queries[q];
                lowIndex = min(lowIndex, min(cur.leftLo, cur.rightLo));
                highIndex = max(highIndex, max(cur.leftHi, cur.rightHi));
                pairQueries += min(cur.leftHi - cur.leftLo + 1,
                                   cur.rightHi - cur.rightLo + 1);
            }

            int width = __builtin_popcount((unsigned)mask);
            long long span = highIndex - lowIndex + 1LL;
            long long directEstimate = pairQueries * directCostPerValue(mask) * 2;
            long long projectedEstimate = span * (width + 1LL) + pairQueries * width * 2LL;

            if (projectedEstimate < directEstimate) {
                ProjectedTrie projected(prefixXor, lowIndex, highIndex, mask);
                for (size_t q = begin; q < end; ++q) {
                    processQueryProjected(queries[q], prefixXor, projected, answer);
                }
            } else {
                for (size_t q = begin; q < end; ++q) {
                    processQueryDirect(queries[q], prefixXor, answer);
                }
            }
            begin = end;
        }

        cout << answer << '\n';
    }
    return 0;
}
