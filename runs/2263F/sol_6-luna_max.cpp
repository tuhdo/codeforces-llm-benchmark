#include <bits/stdc++.h>
using namespace std;

struct TestCaseSolver {
    int n;
    vector<int> a, b;
    vector<vector<int>> graph, reverseGraph;
    vector<int> component;
    vector<vector<int>> members;
    vector<char> markedComponent;
    vector<char> finalized;
    vector<int> currentEdge;
    vector<int> operations;
    int sink = -1;
    int startPos = 1;

    bool routeInsideComponent(int &position, int target, int compId) {
        if (position == target) return true;

        vector<int> parent(n + 1, -1);
        queue<int> q;
        set<int> unseen;
        for (int v : members[compId]) unseen.insert(v);
        unseen.erase(position);
        parent[position] = position;
        q.push(position);

        while (!q.empty() && parent[target] == -1) {
            int v = q.front();
            q.pop();

            int lo, hi;
            if (finalized[v]) {
                lo = hi = b[v];
            } else {
                lo = currentEdge[v];
                hi = b[v];
            }

            // A required self-loop is the final action of the whole walk.
            // It may be used as a transit vertex before then, but must not
            // be switched to its self-loop early.
            if (v == sink && !finalized[v]) hi = min(hi, b[v] - 1);

            auto it = unseen.lower_bound(lo);
            while (it != unseen.end() && *it <= hi) {
                int to = *it;
                it = unseen.erase(it);
                parent[to] = v;
                q.push(to);
                if (to == target) break;
            }
        }

        if (parent[target] == -1) return false;

        vector<int> path;
        for (int v = target; v != position; v = parent[v]) path.push_back(v);
        path.push_back(position);
        reverse(path.begin(), path.end());

        for (int k = 0; k + 1 < (int)path.size(); ++k) {
            int from = path[k];
            int to = path[k + 1];
            if (finalized[from]) {
                if (b[from] != to) return false;
            } else {
                if (currentEdge[from] > to || to > b[from]) return false;
                while (currentEdge[from] < to) {
                    ++currentEdge[from];
                    operations.push_back(1);
                }
                if (currentEdge[from] == b[from]) finalized[from] = true;
            }
            operations.push_back(2);
            position = to;
        }
        return true;
    }

    bool finishVertex(int v, bool follow) {
        if (currentEdge[v] > b[v]) return false;
        while (currentEdge[v] < b[v]) {
            ++currentEdge[v];
            operations.push_back(1);
        }
        finalized[v] = true;
        if (follow) {
            operations.push_back(2);
        }
        return true;
    }

    bool solveOne() {
        cin >> n;
        a.assign(n + 1, 0);
        b.assign(n + 1, 0);
        for (int i = 1; i <= n; ++i) cin >> a[i];
        for (int i = 1; i <= n; ++i) cin >> b[i];

        for (int i = 1; i <= n; ++i) {
            if (a[i] > b[i]) return false;
        }

        vector<int> required;
        for (int i = 1; i <= n; ++i) {
            if (a[i] < b[i]) required.push_back(i);
        }
        if (required.empty()) {
            operations.clear();
            return true;
        }

        int sinkCount = 0;
        for (int i : required) {
            if (b[i] == i) {
                ++sinkCount;
                sink = i;
            }
        }
        if (sinkCount > 1) return false;

        graph.assign(n + 1, {});
        reverseGraph.assign(n + 1, {});
        for (int i = 1; i <= n; ++i) {
            for (int j = a[i]; j <= b[i]; ++j) {
                graph[i].push_back(j);
                reverseGraph[j].push_back(i);
            }
        }

        vector<char> seen(n + 1, false);
        vector<int> order;
        order.reserve(n);
        function<void(int)> dfs1 = [&](int v) {
            seen[v] = true;
            for (int to : graph[v]) if (!seen[to]) dfs1(to);
            order.push_back(v);
        };
        for (int i = 1; i <= n; ++i) if (!seen[i]) dfs1(i);

        component.assign(n + 1, -1);
        members.clear();
        function<void(int, int)> dfs2 = [&](int v, int cid) {
            component[v] = cid;
            members[cid].push_back(v);
            for (int to : reverseGraph[v]) if (component[to] == -1) dfs2(to, cid);
        };
        for (int k = n - 1; k >= 0; --k) {
            int v = order[k];
            if (component[v] == -1) {
                members.push_back({});
                dfs2(v, (int)members.size() - 1);
            }
        }

        int componentCount = (int)members.size();
        markedComponent.assign(componentCount, false);
        vector<int> requiredCount(componentCount, 0);
        for (int i : required) {
            markedComponent[component[i]] = true;
            ++requiredCount[component[i]];
        }

        vector<int> exitVertex(componentCount, -1);
        vector<int> sinkInComponent(componentCount, -1);
        for (int cid = 0; cid < componentCount; ++cid) {
            if (!markedComponent[cid]) continue;
            int exits = 0;
            for (int v : members[cid]) {
                if (component[b[v]] != cid) {
                    ++exits;
                    exitVertex[cid] = v;
                }
                if (v == sink) sinkInComponent[cid] = v;
            }
            if (exits > 1) return false;
            if (sinkInComponent[cid] != -1 && exits != 0) return false;
            if (exits == 0 && sinkInComponent[cid] == -1) return false;
        }

        // From a component with one target-road exit, the target road and
        // the unchanged vertices after it determine the next marked group.
        vector<int> nextMarked(componentCount, -1);
        for (int cid = 0; cid < componentCount; ++cid) {
            if (!markedComponent[cid] || exitVertex[cid] == -1) continue;
            int v = b[exitVertex[cid]];
            vector<char> localSeen(n + 1, false);
            while (!markedComponent[component[v]] && !localSeen[v]) {
                localSeen[v] = true;
                if (b[v] == v) break;
                v = b[v];
            }
            if (markedComponent[component[v]]) nextMarked[cid] = component[v];
        }

        vector<int> indegree(componentCount, 0);
        for (int cid = 0; cid < componentCount; ++cid) {
            if (markedComponent[cid] && nextMarked[cid] != -1)
                ++indegree[nextMarked[cid]];
        }
        int startComponent = -1;
        int markedTotal = 0;
        for (int cid = 0; cid < componentCount; ++cid) {
            if (!markedComponent[cid]) continue;
            ++markedTotal;
            if (indegree[cid] == 0) {
                if (startComponent != -1) return false;
                startComponent = cid;
            }
        }
        if (startComponent == -1) return false;

        vector<int> componentOrder;
        vector<char> inChain(componentCount, false);
        for (int cid = startComponent; cid != -1; cid = nextMarked[cid]) {
            if (inChain[cid]) return false;
            inChain[cid] = true;
            componentOrder.push_back(cid);
        }
        if ((int)componentOrder.size() != markedTotal) return false;

        if (sink != -1) {
            if (componentOrder.back() != component[sink]) return false;
        }

        currentEdge = a;
        finalized.assign(n + 1, false);
        for (int i = 1; i <= n; ++i) finalized[i] = (a[i] == b[i]);
        operations.clear();

        int position = -1;
        int first = componentOrder.front();
        vector<int> firstTasks;
        for (int v : members[first]) {
            if (a[v] < b[v] && v != sink && v != exitVertex[first])
                firstTasks.push_back(v);
        }
        sort(firstTasks.begin(), firstTasks.end());
        if (!firstTasks.empty()) position = firstTasks.front();
        else if (exitVertex[first] != -1) position = exitVertex[first];
        else position = sinkInComponent[first];
        startPos = position;

        for (int idx = 0; idx < (int)componentOrder.size(); ++idx) {
            int cid = componentOrder[idx];
            if (idx > 0) {
                int expected = cid;
                while (component[position] != expected) {
                    int here = component[position];
                    if (markedComponent[here] || a[position] != b[position]) return false;
                    if (b[position] == position) return false;
                    operations.push_back(2);
                    position = b[position];
                }
            }

            vector<int> tasks;
            for (int v : members[cid]) {
                if (a[v] < b[v] && v != sink && v != exitVertex[cid])
                    tasks.push_back(v);
            }
            sort(tasks.begin(), tasks.end());

            for (int v : tasks) {
                if (finalized[v]) continue;
                if (!routeInsideComponent(position, v, cid)) return false;
                if (!finishVertex(v, true)) return false;
                position = b[v];
                if (component[position] != cid) return false;
            }

            if (exitVertex[cid] != -1) {
                int v = exitVertex[cid];
                if (!finalized[v]) {
                    if (!routeInsideComponent(position, v, cid)) return false;
                    if (!finishVertex(v, true)) return false;
                } else {
                    // A required external-exit vertex cannot have been
                    // finalized while staying inside its component.
                    return false;
                }
                position = b[v];

                if (idx + 1 < (int)componentOrder.size()) {
                    int nextCid = componentOrder[idx + 1];
                    int guard = 0;
                    while (component[position] != nextCid) {
                        int here = component[position];
                        if (markedComponent[here] || a[position] != b[position] || b[position] == position)
                            return false;
                        operations.push_back(2);
                        position = b[position];
                        if (++guard > n) return false;
                    }
                }
            } else {
                int v = sinkInComponent[cid];
                if (v == -1) return false;
                if (!finalized[v]) {
                    if (!routeInsideComponent(position, v, cid)) return false;
                    if (!finishVertex(v, false)) return false;
                }
            }
        }

        for (int i = 1; i <= n; ++i) {
            if (currentEdge[i] != b[i]) return false;
        }
        if ((int)operations.size() > 2 * n * n) return false;
        return true;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        TestCaseSolver solver;
        bool ok = solver.solveOne();
        if (!ok) {
            cout << -1 << '\n';
            continue;
        }
        cout << solver.operations.size() << ' ';
        if (solver.operations.empty()) {
            cout << 1 << '\n' << '\n';
        } else {
            // The starting position is the first location used by the
            // construction. For non-empty solutions it is stored below.
            // solveOne sets it in operations construction via startPos.
            cout << solver.startPos << '\n';
            for (int i = 0; i < (int)solver.operations.size(); ++i) {
                if (i) cout << ' ';
                cout << solver.operations[i];
            }
            cout << '\n';
        }
    }
    return 0;
}
