#include <bits/stdc++.h>
using namespace std;

struct PredecessorSet {
    vector<int> parent;

    explicit PredecessorSet(int n = 0) : parent(n + 1) {}

    void reset(int n) {
        parent.resize(n + 1);
        iota(parent.begin(), parent.end(), 0);
    }

    int find(int x) {
        int root = x;
        while (parent[root] != root) root = parent[root];
        while (parent[x] != x) {
            int next = parent[x];
            parent[x] = root;
            x = next;
        }
        return root;
    }

    void erase(int x) { parent[x] = find(x - 1); }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n;
        cin >> n;
        vector<int> p(n + 1), next_greater(n + 1, 0), stack;
        for (int i = 1; i <= n; ++i) cin >> p[i];

        for (int i = n; i >= 1; --i) {
            while (!stack.empty() && p[stack.back()] < p[i]) stack.pop_back();
            if (!stack.empty()) next_greater[i] = stack.back();
            stack.push_back(i);
        }

        long long answer = 0;
        vector<int> distance(n + 1), queue;
        queue.reserve(n);
        PredecessorSet unvisited(n);

        for (int source = 1; source <= n; ++source) {
            fill(distance.begin(), distance.end(), -1);
            unvisited.reset(n);
            unvisited.erase(source);
            queue.clear();
            queue.push_back(source);
            distance[source] = 0;

            for (size_t head = 0; head < queue.size(); ++head) {
                int u = queue[head];
                int candidate = unvisited.find(u - 1);
                while (candidate > 0) {
                    distance[candidate] = distance[u] + 1;
                    answer += distance[candidate];
                    queue.push_back(candidate);
                    unvisited.erase(candidate);
                    candidate = unvisited.find(candidate);
                }

                int v = next_greater[u];
                if (v != 0 && distance[v] == -1) {
                    distance[v] = distance[u] + 1;
                    answer += distance[v];
                    queue.push_back(v);
                    unvisited.erase(v);
                }
            }
        }

        cout << answer << '\n';
    }
    return 0;
}
