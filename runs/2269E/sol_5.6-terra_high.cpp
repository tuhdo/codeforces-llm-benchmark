#include <bits/stdc++.h>
using namespace std;

static constexpr int BITS = 18;
static constexpr int VALUE_COUNT = 1 << BITS;

struct Solver {
    int n;
    vector<int> a, pref;
    vector<int> left_child, right_child, subtree_size, leftmost, rightmost;
    vector<int> frequency, touched;
    int root;

    void build_cartesian_tree() {
        left_child.assign(n, -1);
        right_child.assign(n, -1);
        vector<int> stack;

        for (int i = 0; i < n; ++i) {
            int last = -1;
            while (!stack.empty() && a[stack.back()] < a[i]) {
                last = stack.back();
                stack.pop_back();
            }
            if (!stack.empty()) right_child[stack.back()] = i;
            if (last != -1) left_child[i] = last;
            stack.push_back(i);
        }
        root = stack.front();

        subtree_size.assign(n, 1);
        leftmost.resize(n);
        rightmost.resize(n);

        vector<pair<int, bool>> todo = {{root, false}};
        while (!todo.empty()) {
            auto [v, done] = todo.back();
            todo.pop_back();
            if (!done) {
                todo.push_back({v, true});
                if (left_child[v] != -1) todo.push_back({left_child[v], false});
                if (right_child[v] != -1) todo.push_back({right_child[v], false});
                continue;
            }

            leftmost[v] = rightmost[v] = v;
            if (left_child[v] != -1) {
                int u = left_child[v];
                subtree_size[v] += subtree_size[u];
                leftmost[v] = leftmost[u];
            }
            if (right_child[v] != -1) {
                int u = right_child[v];
                subtree_size[v] += subtree_size[u];
                rightmost[v] = rightmost[u];
            }
        }
    }

    void add(int value) {
        if (frequency[value]++ == 0) touched.push_back(value);
    }

    void remove(int value) {
        --frequency[value];
    }

    void add_range(int l, int r, int mask) {
        for (int i = l; i <= r; ++i) add(pref[i] & mask);
    }

    void remove_range(int l, int r, int mask) {
        for (int i = l; i <= r; ++i) remove(pref[i] & mask);
    }

    bool feasible(int mask) {
        struct Frame {
            int v;
            int stage;
            bool small_is_left;
        };

        bool found = false;
        vector<Frame> todo = {{root, 0, false}};

        while (!todo.empty()) {
            Frame &frame = todo.back();
            int v = frame.v;
            int left_size = left_child[v] == -1 ? 0 : subtree_size[left_child[v]];
            int right_size = right_child[v] == -1 ? 0 : subtree_size[right_child[v]];

            // For an array interval [L, R] in 0-based indexing, its prefix
            // endpoints are [L, R + 1]. The two sides around v are [L, v]
            // and [v + 1, R + 1].
            int left_l = leftmost[v];
            int left_r = v;
            int right_l = v + 1;
            int right_r = rightmost[v] + 1;

            if (frame.stage == 0) {
                frame.small_is_left = (left_size <= right_size);
                frame.stage = 1;
                int child = frame.small_is_left ? left_child[v] : right_child[v];
                if (child == -1) {
                    add(pref[frame.small_is_left ? left_l : right_l] & mask);
                } else {
                    todo.push_back({child, 0, false});
                }
                continue;
            }

            if (frame.stage == 1) {
                if (frame.small_is_left) remove_range(left_l, left_r, mask);
                else remove_range(right_l, right_r, mask);

                frame.stage = 2;
                int child = frame.small_is_left ? right_child[v] : left_child[v];
                if (child == -1) {
                    add(pref[frame.small_is_left ? right_l : left_l] & mask);
                } else {
                    todo.push_back({child, 0, false});
                }
                continue;
            }

            if ((a[v] & mask) == mask) {
                if (frame.small_is_left) {
                    for (int x = left_l; x <= left_r; ++x) {
                        int sx = pref[x] & mask;
                        int need = sx ^ mask;
                        int matches = frequency[need];
                        // Exclude only the one-element interval [v, v].
                        if (x == v && need == (pref[v + 1] & mask)) --matches;
                        if (matches > 0) found = true;
                    }
                } else {
                    for (int y = right_l; y <= right_r; ++y) {
                        int sy = pref[y] & mask;
                        int need = sy ^ mask;
                        int matches = frequency[need];
                        // Exclude only the one-element interval [v, v].
                        if (y == v + 1 && need == (pref[v] & mask)) --matches;
                        if (matches > 0) found = true;
                    }
                }
            }

            if (frame.small_is_left) add_range(left_l, left_r, mask);
            else add_range(right_l, right_r, mask);
            todo.pop_back();
        }

        for (int value : touched) frequency[value] = 0;
        touched.clear();
        return found;
    }

    int solve() {
        build_cartesian_tree();
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

    int tests;
    cin >> tests;
    while (tests--) {
        Solver solver;
        cin >> solver.n;
        solver.a.resize(solver.n);
        solver.pref.assign(solver.n + 1, 0);
        for (int i = 0; i < solver.n; ++i) {
            cin >> solver.a[i];
            solver.pref[i + 1] = solver.pref[i] ^ solver.a[i];
        }
        solver.frequency.assign(VALUE_COUNT, 0);
        cout << solver.solve() << '\n';
    }
}
