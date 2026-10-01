#include <bits/stdc++.h>

using namespace std;

using int64 = long long;

namespace {

constexpr int LOG = 20;

struct VirtualEdge {
    int to;
    int length;
};

}  // namespace

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int test_count;
    cin >> test_count;

    while (test_count--) {
        int n;
        cin >> n;

        vector<vector<int>> vertices_of_color(n + 1);
        for (int vertex = 1; vertex <= n; ++vertex) {
            int color;
            cin >> color;
            vertices_of_color[color].push_back(vertex);
        }

        vector<int> target_size(n + 1);
        for (int color = 1; color <= n; ++color) {
            cin >> target_size[color];
        }

        vector<vector<int>> tree(n + 1);
        for (int edge = 1; edge < n; ++edge) {
            int u, v;
            cin >> u >> v;
            tree[u].push_back(v);
            tree[v].push_back(u);
        }

        vector<array<int, LOG>> up(n + 1);
        vector<int> parent(n + 1);
        vector<int> depth(n + 1);
        vector<int> tin(n + 1);
        vector<int> tout(n + 1);
        vector<int> next_edge(n + 1);

        parent[1] = 1;
        for (int jump = 0; jump < LOG; ++jump) {
            up[1][jump] = 1;
        }

        int timer = 0;
        vector<int> dfs_stack;
        dfs_stack.reserve(n);
        dfs_stack.push_back(1);

        while (!dfs_stack.empty()) {
            int vertex = dfs_stack.back();

            if (next_edge[vertex] == 0) {
                tin[vertex] = timer++;
            }

            if (next_edge[vertex] == static_cast<int>(tree[vertex].size())) {
                tout[vertex] = timer;
                dfs_stack.pop_back();
                continue;
            }

            int to = tree[vertex][next_edge[vertex]++];
            if (to == parent[vertex]) {
                continue;
            }

            parent[to] = vertex;
            depth[to] = depth[vertex] + 1;
            up[to][0] = vertex;
            for (int jump = 1; jump < LOG; ++jump) {
                up[to][jump] = up[up[to][jump - 1]][jump - 1];
            }
            dfs_stack.push_back(to);
        }

        auto is_ancestor = [&](int ancestor, int vertex) {
            return tin[ancestor] <= tin[vertex] && tout[vertex] <= tout[ancestor];
        };

        auto lca = [&](int a, int b) {
            if (is_ancestor(a, b)) {
                return a;
            }
            if (is_ancestor(b, a)) {
                return b;
            }
            for (int jump = LOG - 1; jump >= 0; --jump) {
                if (!is_ancestor(up[a][jump], b)) {
                    a = up[a][jump];
                }
            }
            return up[a][0];
        };

        vector<int64> answer(n + 1, -1);

        for (int color = 1; color <= n; ++color) {
            vector<int>& marked = vertices_of_color[color];
            if (marked.empty()) {
                continue;
            }

            if (marked.size() == 1) {
                answer[color] = 0;
                continue;
            }

            sort(marked.begin(), marked.end(), [&](int a, int b) {
                return tin[a] < tin[b];
            });

            vector<int> virtual_vertices = marked;
            virtual_vertices.reserve(marked.size() * 2);
            for (int index = 1; index < static_cast<int>(marked.size()); ++index) {
                virtual_vertices.push_back(lca(marked[index - 1], marked[index]));
            }

            sort(virtual_vertices.begin(), virtual_vertices.end(), [&](int a, int b) {
                return tin[a] < tin[b];
            });
            virtual_vertices.erase(
                unique(virtual_vertices.begin(), virtual_vertices.end()),
                virtual_vertices.end());

            int virtual_count = static_cast<int>(virtual_vertices.size());
            vector<vector<VirtualEdge>> virtual_tree(virtual_count);
            vector<int> degree(virtual_count);
            int64 steiner_size = 1;

            vector<int> stack;
            stack.reserve(virtual_count);
            for (int index = 0; index < virtual_count; ++index) {
                while (!stack.empty() &&
                       !is_ancestor(virtual_vertices[stack.back()], virtual_vertices[index])) {
                    stack.pop_back();
                }

                if (!stack.empty()) {
                    int par = stack.back();
                    int length = depth[virtual_vertices[index]] - depth[virtual_vertices[par]];
                    virtual_tree[index].push_back({par, length});
                    virtual_tree[par].push_back({index, length});
                    ++degree[index];
                    ++degree[par];
                    steiner_size += length;
                }
                stack.push_back(index);
            }

            vector<int64> mass(virtual_count);
            int marked_index = 0;
            for (int index = 0; index < virtual_count; ++index) {
                if (marked_index < static_cast<int>(marked.size()) &&
                    virtual_vertices[index] == marked[marked_index]) {
                    mass[index] = 1;
                    ++marked_index;
                }
            }

            int64 vertices_to_remove = steiner_size - target_size[color];
            int64 cost = 0;

            priority_queue<pair<int64, int>, vector<pair<int64, int>>, greater<>> leaves;
            for (int index = 0; index < virtual_count; ++index) {
                if (degree[index] == 1) {
                    leaves.push({mass[index], index});
                }
            }

            vector<char> alive(virtual_count, true);
            while (vertices_to_remove > 0) {
                auto [leaf_mass, leaf] = leaves.top();
                leaves.pop();

                if (!alive[leaf] || degree[leaf] != 1 || mass[leaf] != leaf_mass) {
                    continue;
                }

                int neighbor = -1;
                int edge_length = 0;
                for (const VirtualEdge& edge : virtual_tree[leaf]) {
                    if (alive[edge.to]) {
                        neighbor = edge.to;
                        edge_length = edge.length;
                        break;
                    }
                }

                int64 removed_here = min<int64>(vertices_to_remove, edge_length);
                cost += removed_here * leaf_mass;
                vertices_to_remove -= removed_here;

                if (vertices_to_remove == 0) {
                    break;
                }

                alive[leaf] = false;
                degree[leaf] = 0;
                --degree[neighbor];
                mass[neighbor] += leaf_mass;

                if (degree[neighbor] == 1) {
                    leaves.push({mass[neighbor], neighbor});
                }
            }

            answer[color] = cost;
        }

        for (int color = 1; color <= n; ++color) {
            cout << answer[color] << " \n"[color == n];
        }
    }

    return 0;
}
