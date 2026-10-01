#include <algorithm>
#include <cstdio>
#include <vector>

using namespace std;

class FastInput {
    static constexpr int SIZE = 1 << 16;
    char buffer[SIZE];
    int position = 0, length = 0;

    int getChar() {
        if (position == length) {
            length = static_cast<int>(fread(buffer, 1, SIZE, stdin));
            position = 0;
            if (length == 0) return EOF;
        }
        return buffer[position++];
    }

public:
    int readInt() {
        int c = getChar();
        while (c <= ' ' && c != EOF) c = getChar();
        int value = 0;
        while (c >= '0' && c <= '9') {
            value = value * 10 + c - '0';
            c = getChar();
        }
        return value;
    }
};

class FastOutput {
    static constexpr int SIZE = 1 << 16;
    char buffer[SIZE];
    int position = 0;

public:
    ~FastOutput() { flush(); }

    void flush() {
        fwrite(buffer, 1, position, stdout);
        position = 0;
    }

    void putChar(char c) {
        if (position == SIZE) flush();
        buffer[position++] = c;
    }

    void writeInt(int value) {
        char digits[12];
        int count = 0;
        do {
            digits[count++] = static_cast<char>('0' + value % 10);
            value /= 10;
        } while (value != 0);
        while (count != 0) putChar(digits[--count]);
    }
};

struct Tree {
    vector<int> head, to, next;
    int edgeCount = 0;

    explicit Tree(int n) : head(n, -1), to(2 * (n - 1)), next(2 * (n - 1)) {}

    void addEdge(int u, int v) {
        to[edgeCount] = v;
        next[edgeCount] = head[u];
        head[u] = edgeCount++;
    }
};

int traverse(const Tree& tree, int root, int blocked, vector<int>& parent,
             vector<int>& depth, vector<int>& order) {
    order.clear();
    order.push_back(root);
    parent[root] = blocked;
    depth[root] = 0;

    for (int i = 0; i < static_cast<int>(order.size()); ++i) {
        int u = order[i];
        for (int e = tree.head[u]; e != -1; e = tree.next[e]) {
            int v = tree.to[e];
            if (v == parent[u]) continue;
            parent[v] = u;
            depth[v] = depth[u] + 1;
            order.push_back(v);
        }
    }
    return order.back();
}

vector<int> possibleLengths(const Tree& tree, int center, int otherCenter,
                            int height, vector<int>& parent, vector<int>& depth,
                            vector<int>& order, vector<unsigned char>& activeChildren) {
    traverse(tree, center, otherCenter, parent, depth, order);
    vector<unsigned char> possible(height + 1, 0);
    possible[height] = 1;  // Choosing the same endpoint on this side.

    for (int i = static_cast<int>(order.size()) - 1; i >= 0; --i) {
        int u = order[i];
        // Two children containing deepest endpoints make u a possible LCA.
        if (activeChildren[u] == 2) possible[depth[u]] = 1;

        if (u != center && (depth[u] == height || activeChildren[u] != 0)) {
            auto& count = activeChildren[parent[u]];
            if (count < 2) ++count;
        }
    }

    vector<int> lengths;
    for (int d = 0; d <= height; ++d) {
        if (possible[d]) lengths.push_back(d);
    }
    return lengths;
}

int main() {
    FastInput input;
    FastOutput output;

    int tests = input.readInt();
    while (tests--) {
        int n = input.readInt();
        Tree tree(n);
        for (int i = 0; i < n - 1; ++i) {
            int u = input.readInt() - 1;
            int v = input.readInt() - 1;
            tree.addEdge(u, v);
            tree.addEdge(v, u);
        }

        vector<int> parent(n), depth(n), order;
        order.reserve(n);
        int endpoint = traverse(tree, 0, -1, parent, depth, order);
        int otherEndpoint = traverse(tree, endpoint, -1, parent, depth, order);
        int diameter = depth[otherEndpoint];
        int height = diameter / 2;

        int rightCenter = otherEndpoint;
        for (int i = 0; i < height; ++i) rightCenter = parent[rightCenter];
        int leftCenter = parent[rightCenter];

        // The two calls visit disjoint components, so their counts start at zero.
        vector<unsigned char> activeChildren(n, 0);
        vector<int> left = possibleLengths(tree, leftCenter, rightCenter, height,
                                          parent, depth, order, activeChildren);
        vector<int> right = possibleLengths(tree, rightCenter, leftCenter, height,
                                           parent, depth, order, activeChildren);

        vector<unsigned char> beautiful(diameter + 1, 0);
        // A side with s possible lengths contains at least s(s+1)/2 vertices.
        // Therefore left.size() * right.size() <= n, making this loop linear.
        for (int x : left) {
            for (int y : right) beautiful[1 + x + y] = 1;
        }

        int count = 0;
        for (int k = 1; k <= diameter; ++k) count += beautiful[k];
        output.writeInt(count);
        output.putChar('\n');
        bool first = true;
        for (int k = 1; k <= diameter; ++k) {
            if (!beautiful[k]) continue;
            if (!first) output.putChar(' ');
            first = false;
            output.writeInt(k);
        }
        output.putChar('\n');
    }
    return 0;
}
