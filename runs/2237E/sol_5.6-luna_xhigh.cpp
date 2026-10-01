#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;

        vector<int> a(n), b(n);
        for (int &x : a) {
            cin >> x;
            --x;
        }
        for (int &x : b) {
            cin >> x;
            if (x != -1) --x;
        }

        vector<int> cycle_id(n, -1), position(n, -1);
        vector<vector<int>> cycles;
        vector<char> visited(n, false);

        for (int start = 0; start < n; ++start) {
            if (visited[start]) continue;

            int id = (int)cycles.size();
            vector<int> cycle;
            int v = start;
            while (!visited[v]) {
                visited[v] = true;
                cycle_id[v] = id;
                position[v] = (int)cycle.size();
                cycle.push_back(v);
                v = a[v];
            }
            cycles.push_back(move(cycle));
        }

        int m = (int)cycles.size();
        vector<int> target(m, -1), shift(m, 0);
        vector<char> has_assignment(m, false), target_used(m, false);
        bool ok = true;

        for (int i = 0; i < n; ++i) {
            if (b[i] == -1) continue;

            int source = cycle_id[i];
            int destination = cycle_id[b[i]];
            int len = (int)cycles[source].size();

            if ((int)cycles[destination].size() != len) {
                ok = false;
                continue;
            }

            int current_shift = position[b[i]] - position[i];
            current_shift %= len;
            if (current_shift < 0) current_shift += len;

            if (!has_assignment[source]) {
                target[source] = destination;
                shift[source] = current_shift;
                has_assignment[source] = true;
            } else if (target[source] != destination || shift[source] != current_shift) {
                ok = false;
            }
        }

        for (int source = 0; source < m; ++source) {
            if (!has_assignment[source]) continue;
            if (target_used[target[source]]) {
                ok = false;
            } else {
                target_used[target[source]] = true;
            }
        }

        vector<int> minimum_element(m, n);
        vector<vector<int>> by_length(n + 1);
        for (int id = 0; id < m; ++id) {
            for (int v : cycles[id]) minimum_element[id] = min(minimum_element[id], v);
            by_length[cycles[id].size()].push_back(id);
        }

        for (auto &group : by_length) {
            sort(group.begin(), group.end(), [&](int x, int y) {
                return minimum_element[x] < minimum_element[y];
            });
        }

        if (ok) {
            vector<int> next_target(n + 1, 0);
            for (int first = 0; first < n; ++first) {
                int source = cycle_id[first];
                if (minimum_element[source] != first || has_assignment[source]) continue;

                int len = (int)cycles[source].size();
                auto &group = by_length[len];
                while (next_target[len] < (int)group.size() && target_used[group[next_target[len]]]) {
                    ++next_target[len];
                }

                if (next_target[len] == (int)group.size()) {
                    ok = false;
                    break;
                }

                int destination = group[next_target[len]++];
                target_used[destination] = true;
                target[source] = destination;

                int destination_min_position = position[minimum_element[destination]];
                int source_min_position = position[first];
                shift[source] = destination_min_position - source_min_position;
                shift[source] %= len;
                if (shift[source] < 0) shift[source] += len;
            }
        }

        if (!ok) {
            cout << "NO\n";
            continue;
        }

        vector<int> answer(n);
        for (int source = 0; source < m; ++source) {
            int destination = target[source];
            int len = (int)cycles[source].size();
            for (int v : cycles[source]) {
                int source_position = position[v];
                int destination_position = (source_position + shift[source]) % len;
                answer[v] = cycles[destination][destination_position];
            }
        }

        cout << "YES\n";
        for (int i = 0; i < n; ++i) {
            if (i) cout << ' ';
            cout << answer[i] + 1;
        }
        cout << '\n';
    }

    return 0;
}
