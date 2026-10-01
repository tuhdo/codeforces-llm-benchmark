#include <bits/stdc++.h>
using namespace std;

class GapQueue {
    int n;
    int size;
    int head = 0;
    vector<int> tree;
    vector<int> value;

    void setValue(int index, int newValue) {
        value[index] = newValue;
        int p = size + index;
        tree[p] = newValue;
        for (p >>= 1; p > 0; p >>= 1)
            tree[p] = tree[p << 1] + tree[p << 1 | 1];
    }

    int sum(int left, int right) const {
        int result = 0;
        for (left += size, right += size; left < right; left >>= 1, right >>= 1) {
            if (left & 1) result += tree[left++];
            if (right & 1) result += tree[--right];
        }
        return result;
    }

    int findFirst(int node, int left, int right, int ql, int qr) const {
        if (right <= ql || qr <= left || tree[node] == 0) return -1;
        if (right - left == 1) return left;
        int mid = (left + right) / 2;
        int result = findFirst(node << 1, left, mid, ql, qr);
        if (result != -1) return result;
        return findFirst(node << 1 | 1, mid, right, ql, qr);
    }

public:
    explicit GapQueue(const vector<int>& gaps)
        : n((int)gaps.size()), size(1) {
        while (size < max(1, n)) size <<= 1;
        tree.assign(size * 2, 0);
        value = gaps;
        for (int i = 0; i < n; ++i) tree[size + i] = value[i];
        for (int i = size - 1; i > 0; --i)
            tree[i] = tree[i << 1] + tree[i << 1 | 1];
    }

    // Remove one one from the first nonempty gap before a zero. Returns its
    // logical gap index, or -1 if every one is after the last zero.
    int takeFromFirstGap() {
        int physical = -1;
        if (sum(head, n) > 0)
            physical = findFirst(1, 0, size, head, n);
        else if (head > 0)
            physical = findFirst(1, 0, size, 0, head);
        if (physical == -1) return -1;

        int logical = (physical - head + n) % n;
        setValue(physical, value[physical] - 1);
        return logical;
    }

    // A reverse bubble shifts the gap queue right by one and transfers the
    // dropped last gap into the trailing ones.
    int shiftRight() {
        int droppedIndex = (head + n - 1) % n;
        int dropped = value[droppedIndex];
        head = (head + n - 1) % n;
        setValue(droppedIndex, 0);
        return dropped;
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
        vector<int> a(n);
        for (int& x : a) cin >> x;
        string s;
        cin >> s;

        int zeroCount = 0;
        vector<int> gaps(n + 1, 0);
        long long inversions = 0;
        long long onesSeen = 0;
        for (int x : a) {
            if (x == 0) {
                ++zeroCount;
                inversions += onesSeen;
            } else {
                ++gaps[zeroCount];
                ++onesSeen;
            }
        }

        // The first zeroCount gaps are before or between zeros; the final
        // gap contains ones after the last zero.
        vector<int> queuedGaps(gaps.begin(), gaps.begin() + zeroCount);
        long long trailingOnes = gaps[zeroCount];
        GapQueue queue(queuedGaps);

        cout << inversions;
        for (char op : s) {
            if (op == '1') {
                int gap = queue.takeFromFirstGap();
                if (gap != -1) {
                    inversions -= zeroCount - gap;
                    ++trailingOnes;
                }
            } else if (zeroCount > 0) {
                long long onesBeforeLastZero = onesSeen - trailingOnes;
                inversions -= onesBeforeLastZero;
                trailingOnes += queue.shiftRight();
            }
            cout << ' ' << inversions;
        }
        cout << '\n';
    }
    return 0;
}
