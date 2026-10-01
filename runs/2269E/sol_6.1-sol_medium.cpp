#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

class Solver {
    static constexpr int BITS = 18;
    struct Frame {
        int node;
        bool keep;
        bool merge;
    };

    int n;
    vector<int> a, prefix, left, right, lo, hi, heavy, light;
    vector<int> masked;
    inline static int frequency[1 << BITS] = {};
    vector<Frame> frames;
    int root;

    bool feasible(int mask) {
        for (int i = 0; i <= n; ++i) masked[i] = prefix[i] & mask;
        frames.clear();
        frames.push_back({root, true, false});
        bool found = false;

        while (!frames.empty()) {
            Frame frame = frames.back();
            frames.pop_back();
            int v = frame.node;

            if (v > n) {
                if (frame.keep) ++frequency[masked[lo[v]]];
                continue;
            }

            if (!frame.merge) {
                // Solve the light subtree, discard it, then retain the heavy one.
                frames.push_back({v, frame.keep, true});
                frames.push_back({heavy[v], true, false});
                frames.push_back({light[v], false, false});
                continue;
            }

            int small = light[v];
            if ((a[v] & mask) == mask) {
                // The only forbidden cross pair is (v - 1, v), a singleton.
                int singletonEndpoint = small == left[v] ? v - 1 : v;
                for (int j = lo[small]; j <= hi[small]; ++j) {
                    int matches = frequency[masked[j] ^ mask];
                    if (matches > (j == singletonEndpoint)) {
                        found = true;
                        break;
                    }
                }
            }
            if (found) break;

            for (int j = lo[small]; j <= hi[small]; ++j)
                ++frequency[masked[j]];

            if (!frame.keep) {
                for (int j = lo[v]; j <= hi[v]; ++j)
                    --frequency[masked[j]];
            }
        }
        // Only projected prefix values were inserted, even on an early exit.
        for (int value : masked) frequency[value] = 0;
        return found;
    }

public:
    explicit Solver(vector<int> values)
        : n(static_cast<int>(values.size()) - 1), a(move(values)),
          prefix(n + 1), left(2 * n + 2), right(2 * n + 2),
          lo(2 * n + 2), hi(2 * n + 2), heavy(n + 1), light(n + 1),
          masked(n + 1) {
        vector<int> stack;
        stack.reserve(n);
        for (int i = 1; i <= n; ++i) {
            prefix[i] = prefix[i - 1] ^ a[i];
            int last = 0;
            while (!stack.empty() && a[stack.back()] < a[i]) {
                last = stack.back();
                stack.pop_back();
            }
            if (!stack.empty()) right[stack.back()] = i;
            left[i] = last;
            stack.push_back(i);
        }
        root = stack.front();

        // Turn missing Cartesian-tree children into prefix-XOR leaves.
        for (int i = 1; i <= n; ++i) {
            if (!left[i]) left[i] = n + i;
            if (!right[i]) right[i] = n + i + 1;
        }
        for (int j = 0; j <= n; ++j)
            lo[n + 1 + j] = hi[n + 1 + j] = j;

        vector<int> order;
        order.reserve(n);
        order.push_back(root);
        for (int k = 0; k < static_cast<int>(order.size()); ++k) {
            int v = order[k];
            if (left[v] <= n) order.push_back(left[v]);
            if (right[v] <= n) order.push_back(right[v]);
        }
        for (int k = n - 1; k >= 0; --k) {
            int v = order[k];
            lo[v] = lo[left[v]];
            hi[v] = hi[right[v]];
            heavy[v] = left[v];
            light[v] = right[v];
            if (hi[heavy[v]] - lo[heavy[v]] < hi[light[v]] - lo[light[v]])
                swap(heavy[v], light[v]);
        }
        frames.reserve(2 * n + 1);
    }

    int solve() {
        int answer = 0;
        for (int bit = BITS - 1; bit >= 0; --bit) {
            int candidate = answer | (1 << bit);
            if (feasible(candidate)) answer = candidate;
        }
        return answer;
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
        vector<int> a(n + 1);
        for (int i = 1; i <= n; ++i) cin >> a[i];
        cout << Solver(move(a)).solve() << '\n';
    }
}
