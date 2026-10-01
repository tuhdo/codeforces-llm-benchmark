#include <bits/stdc++.h>
using namespace std;

struct Fenwick {
    int n;
    vector<int> bit;

    explicit Fenwick(int n) : n(n), bit(n + 1, 0) {}

    void add(int index, int value) {
        for (int i = index + 1; i <= n; i += i & -i) {
            bit[i] += value;
        }
    }

    // Number of active indices in [0, end).
    int sumPrefix(int end) const {
        int result = 0;
        for (int i = end; i > 0; i -= i & -i) {
            result += bit[i];
        }
        return result;
    }

    int findByOrder(int order) const {
        int index = 0;
        int step = 1;
        while ((step << 1) <= n) step <<= 1;
        for (; step > 0; step >>= 1) {
            int next = index + step;
            if (next <= n && bit[next] < order) {
                index = next;
                order -= bit[next];
            }
        }
        return index; // zero-based index of the requested active item
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<int> p(n);
        for (int &x : p) cin >> x;

        // parent[i] is the nearest greater element to the right.
        vector<int> parent(n, -1), stack;
        stack.reserve(n);
        for (int i = n - 1; i >= 0; --i) {
            while (!stack.empty() && p[stack.back()] < p[i]) {
                stack.pop_back();
            }
            if (!stack.empty()) parent[i] = stack.back();
            stack.push_back(i);
        }

        vector<int> depth(n, 0), childCount(n, 0);
        for (int i = n - 1; i >= 0; --i) {
            if (parent[i] != -1) {
                depth[i] = depth[parent[i]] + 1;
                ++childCount[parent[i]];
            }
        }

        // Store each parent's children contiguously, in increasing index order.
        vector<int> offset(n + 1, 0);
        for (int i = 0; i < n; ++i) {
            offset[i + 1] = offset[i] + childCount[i];
        }
        vector<int> children(offset[n]);
        vector<int> cursor = offset;
        for (int i = 0; i < n; ++i) {
            if (parent[i] != -1) {
                children[cursor[parent[i]]++] = i;
            }
        }

        // The next-greater forest's subtree at v is a contiguous interval ending at v.
        vector<int> subtreeSize(n, 1);
        for (int i = 0; i < n; ++i) {
            if (parent[i] != -1) {
                subtreeSize[parent[i]] += subtreeSize[i];
            }
        }

        // F[v] = sum over its subtree interval of
        // min(depth[x], 1 + the preceding prefix minimum of depth).
        vector<long long> F(n, 0);
        for (int v = 0; v < n; ++v) {
            if (childCount[v] == 0) {
                F[v] = depth[v];
                continue;
            }

            int firstChild = children[offset[v]];
            long long value = F[firstChild] + depth[v];
            for (int k = offset[v] + 1; k < offset[v + 1]; ++k) {
                int child = children[k];
                value += 1LL * subtreeSize[child] * (depth[v] + 2) - 1;
            }
            F[v] = value;
        }

        Fenwick active(n);
        long long upwardSum = 0;

        for (int target = 0; target < n; ++target) {
            // At this target, edges ending at target-1 expire, and the edge
            // starting at target-1 becomes active.
            if (target > 0) {
                int expiredParent = target - 1;
                for (int k = offset[expiredParent]; k < offset[expiredParent + 1]; ++k) {
                    active.add(children[k], -1);
                }
                int newEdge = target - 1;
                if (parent[newEdge] != -1) active.add(newEdge, 1);
            }

            int activeCount = active.sumPrefix(n);
            if (activeCount == 0) continue;

            int first = active.findByOrder(1);
            int firstCost = (parent[first] == target ? 1 : 2);
            long long firstSize = subtreeSize[first];
            long long contribution = F[first] - firstSize * depth[first] + firstSize * firstCost;

            auto childBegin = children.begin() + offset[target];
            auto childEnd = children.begin() + offset[target + 1];

            if (firstCost == 1) {
                // This first interval has already provided a one-step route.
                // Later exact intervals improve their endpoint by one.
                int exactAfter = int(childEnd - upper_bound(childBegin, childEnd, first));
                contribution += 2LL * (target - 1 - first) - exactAfter;
            } else {
                auto nextExactIt = upper_bound(childBegin, childEnd, first);
                if (nextExactIt == childEnd) {
                    // No exact edge remains; later active intervals are all overshoots.
                    int activeAfter = activeCount - active.sumPrefix(first + 1);
                    contribution += 3LL * (target - 1 - first) - activeAfter;
                } else {
                    int nextExact = *nextExactIt;
                    int overshootsBeforeExact = active.sumPrefix(nextExact)
                                                - active.sumPrefix(first + 1);
                    int exactAfter = int(childEnd - upper_bound(childBegin, childEnd, nextExact));
                    contribution += 3LL * (nextExact - first)
                                  + 2LL * (target - 1 - nextExact)
                                  - overshootsBeforeExact
                                  - (2 + childCount[nextExact])
                                  - exactAfter;
                }
            }

            upwardSum += contribution;
        }

        long long answer = 1LL * n * (n - 1) / 2 + upwardSum;
        cout << answer << '\n';
    }

    return 0;
}
