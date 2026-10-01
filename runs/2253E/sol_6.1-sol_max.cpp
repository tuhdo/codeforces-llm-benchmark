#include <bits/stdc++.h>
using namespace std;

class FastInput {
    static constexpr size_t BUFFER_SIZE = 1 << 16;
    char buffer[BUFFER_SIZE];
    size_t position = 0, size = 0;

    char getChar() {
        if (position == size) {
            size = fread(buffer, 1, BUFFER_SIZE, stdin);
            position = 0;
            if (size == 0) return 0;
        }
        return buffer[position++];
    }

public:
    int readInt() {
        char c;
        do {
            c = getChar();
        } while (c <= ' ' && c != 0);
        int value = 0;
        while (c >= '0' && c <= '9') {
            value = value * 10 + c - '0';
            c = getChar();
        }
        return value;
    }
};

class FastOutput {
    static constexpr size_t BUFFER_SIZE = 1 << 16;
    char buffer[BUFFER_SIZE];
    size_t size = 0;

    void putChar(char c) {
        if (size == BUFFER_SIZE) flush();
        buffer[size++] = c;
    }

public:
    ~FastOutput() { flush(); }

    void flush() {
        fwrite(buffer, 1, size, stdout);
        size = 0;
    }

    void writeInt(int value, char delimiter) {
        char digits[12];
        int count = 0;
        do {
            digits[count++] = char('0' + value % 10);
            value /= 10;
        } while (value != 0);
        while (count != 0) putChar(digits[--count]);
        putChar(delimiter);
    }
};

int main() {
    FastInput input;
    FastOutput output;
    int tests = input.readInt();
    while (tests--) {
        int n = input.readInt();
        vector<int> head(n + 1, -1);
        vector<int> to(2 * (n - 1)), next_edge(2 * (n - 1));
        int edge_count = 0;
        auto addEdge = [&](int u, int v) {
            to[edge_count] = v;
            next_edge[edge_count] = head[u];
            head[u] = edge_count++;
        };
        for (int i = 0; i < n - 1; ++i) {
            int u = input.readInt();
            int v = input.readInt();
            addEdge(u, v);
            addEdge(v, u);
        }

        vector<int> parent(n + 1), depth(n + 1), order;
        order.reserve(n);
        auto farthestVertex = [&](int root) {
            order.clear();
            order.push_back(root);
            parent[root] = 0;
            depth[root] = 0;
            for (size_t i = 0; i < order.size(); ++i) {
                int u = order[i];
                for (int e = head[u]; e != -1; e = next_edge[e]) {
                    int v = to[e];
                    if (v == parent[u]) continue;
                    parent[v] = u;
                    depth[v] = depth[u] + 1;
                    order.push_back(v);
                }
            }
            return order.back();
        };

        int endpoint = farthestVertex(1);
        endpoint = farthestVertex(endpoint);
        int diameter = depth[endpoint];
        int height = diameter / 2;
        int right_center = endpoint;
        for (int i = 0; i < height; ++i) {
            right_center = parent[right_center];
        }
        int left_center = parent[right_center];

        // Count only children whose subtrees contain a vertex at depth height.
        // Two such children are enough to realize this vertex as an LCA.
        vector<unsigned char> active_children(n + 1, 0);
        auto collectDepths = [&](int root, int blocked) {
            order.clear();
            order.push_back(root);
            parent[root] = blocked;
            depth[root] = 0;
            for (size_t i = 0; i < order.size(); ++i) {
                int u = order[i];
                for (int e = head[u]; e != -1; e = next_edge[e]) {
                    int v = to[e];
                    if (v == parent[u]) continue;
                    parent[v] = u;
                    depth[v] = depth[u] + 1;
                    order.push_back(v);
                }
            }

            vector<unsigned char> seen(height + 1, 0);
            vector<int> choices{height};
            seen[height] = 1; // Choosing the same endpoint is allowed.
            for (size_t i = order.size(); i-- > 0;) {
                int u = order[i];
                if (depth[u] != height && active_children[u] == 0) continue;
                if (active_children[u] == 2 && !seen[depth[u]]) {
                    seen[depth[u]] = 1;
                    choices.push_back(depth[u]);
                }
                if (u != root && active_children[parent[u]] < 2) {
                    ++active_children[parent[u]];
                }
            }
            return choices;
        };

        vector<int> left = collectDepths(left_center, right_center);
        vector<int> right = collectDepths(right_center, left_center);
        vector<unsigned char> beautiful(diameter + 1, 0);

        // A list of s distinct depths requires at least s*(s-1)/2 edges.
        // Thus enumerating all pairs takes O(n) time.
        for (int a : left) {
            for (int b : right) {
                beautiful[a + b + 1] = 1;
            }
        }

        int count = 0;
        for (int k = 1; k <= diameter; ++k) count += beautiful[k];
        output.writeInt(count, '\n');
        for (int k = 1; k <= diameter; ++k) {
            if (beautiful[k]) {
                output.writeInt(k, --count == 0 ? '\n' : ' ');
            }
        }
    }
    return 0;
}
