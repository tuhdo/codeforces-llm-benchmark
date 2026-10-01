#include <bits/stdc++.h>
using namespace std;

struct BFSResult {
    int farthest;
    vector<int> parent;
};

static BFSResult farthest_from(int start, const vector<vector<int>>& graph, bool keep_parent) {
    const int n = static_cast<int>(graph.size());
    vector<int> distance(n, -1);
    vector<int> parent;
    if (keep_parent) {
        parent.assign(n, -1);
    }

    vector<int> queue(n);
    int head = 0;
    int tail = 0;
    queue[tail++] = start;
    distance[start] = 0;
    int farthest = start;

    while (head < tail) {
        int vertex = queue[head++];
        if (distance[vertex] > distance[farthest]) {
            farthest = vertex;
        }
        for (int next : graph[vertex]) {
            if (distance[next] != -1) {
                continue;
            }
            distance[next] = distance[vertex] + 1;
            if (keep_parent) {
                parent[next] = vertex;
            }
            queue[tail++] = next;
        }
    }

    return {farthest, move(parent)};
}

// Returns every depth that can be the LCA of two deepest vertices in this component.
static vector<int> possible_lca_depths(int root, int blocked, const vector<vector<int>>& graph) {
    const int n = static_cast<int>(graph.size());
    vector<int> parent(n, -2);
    vector<int> depth(n, 0);
    vector<int> order;
    order.reserve(n);

    parent[root] = blocked;
    vector<int> stack;
    stack.push_back(root);
    int maximum_depth = 0;

    while (!stack.empty()) {
        int vertex = stack.back();
        stack.pop_back();
        order.push_back(vertex);
        maximum_depth = max(maximum_depth, depth[vertex]);

        for (int next : graph[vertex]) {
            if (next == parent[vertex]) {
                continue;
            }
            parent[next] = vertex;
            depth[next] = depth[vertex] + 1;
            stack.push_back(next);
        }
    }

    // has_deepest[v] means that v's rooted subtree contains a vertex at maximum_depth.
    vector<char> has_deepest(n, false);
    vector<char> attainable(maximum_depth + 1, false);

    for (int index = static_cast<int>(order.size()) - 1; index >= 0; --index) {
        int vertex = order[index];
        if (depth[vertex] == maximum_depth) {
            has_deepest[vertex] = true;
        }

        int deep_children = 0;
        for (int next : graph[vertex]) {
            if (parent[next] == vertex && has_deepest[next]) {
                ++deep_children;
                has_deepest[vertex] = true;
            }
        }

        // With one deep child, every deepest endpoint takes the same first edge.
        // With zero children we may choose this deepest vertex twice.
        if (has_deepest[vertex] && deep_children != 1) {
            attainable[depth[vertex]] = true;
        }
    }

    vector<int> result;
    for (int d = 0; d <= maximum_depth; ++d) {
        if (attainable[d]) {
            result.push_back(d);
        }
    }
    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int test_count;
    cin >> test_count;
    while (test_count--) {
        int n;
        cin >> n;
        vector<vector<int>> graph(n);
        for (int i = 0; i + 1 < n; ++i) {
            int u, v;
            cin >> u >> v;
            --u;
            --v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }

        int endpoint_a = farthest_from(0, graph, false).farthest;
        BFSResult second_search = farthest_from(endpoint_a, graph, true);
        int endpoint_b = second_search.farthest;

        int diameter_length = 0;
        for (int vertex = endpoint_b; vertex != endpoint_a; vertex = second_search.parent[vertex]) {
            ++diameter_length;
        }

        int half = diameter_length / 2;
        int center_right = endpoint_b;
        for (int i = 0; i < half; ++i) {
            center_right = second_search.parent[center_right];
        }
        int center_left = second_search.parent[center_right];

        vector<int> left_depths = possible_lca_depths(center_left, center_right, graph);
        vector<int> right_depths = possible_lca_depths(center_right, center_left, graph);

        vector<char> beautiful(diameter_length + 1, false);
        for (int left : left_depths) {
            for (int right : right_depths) {
                beautiful[left + right + 1] = true;
            }
        }

        int count = 0;
        for (bool value : beautiful) {
            count += value;
        }

        cout << count;
        for (int k = 0; k <= diameter_length; ++k) {
            if (beautiful[k]) {
                cout << ' ' << k;
            }
        }
        cout << '\n';
    }
}
