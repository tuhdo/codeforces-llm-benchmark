#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;

    while (t--) {
        int n;
        cin >> n;

        vector<int> a(n + 1), b(n + 1);
        for (int i = 1; i <= n; ++i) {
            cin >> a[i];
        }
        for (int i = 1; i <= n; ++i) {
            cin >> b[i];
        }

        vector<vector<int>> cycles;
        vector<int> cycle_id(n + 1, -1), position(n + 1), visited(n + 1);

        for (int start = 1; start <= n; ++start) {
            if (visited[start]) {
                continue;
            }

            int id = static_cast<int>(cycles.size());
            vector<int> cycle;
            for (int v = start; !visited[v]; v = a[v]) {
                visited[v] = 1;
                cycle_id[v] = id;
                position[v] = static_cast<int>(cycle.size());
                cycle.push_back(v);
            }
            cycles.push_back(move(cycle));
        }

        int cycle_count = static_cast<int>(cycles.size());
        vector<int> target_of(cycle_count, -1);
        vector<int> shift(cycle_count, 0);
        vector<int> owner(cycle_count, -1);
        vector<int> used_value(n + 1, 0);
        bool possible = true;

        for (int i = 1; i <= n; ++i) {
            if (b[i] == -1) {
                continue;
            }

            int value = b[i];
            if (value < 1 || value > n || used_value[value]) {
                possible = false;
                continue;
            }
            used_value[value] = 1;

            int source = cycle_id[i];
            int target = cycle_id[value];
            int length = static_cast<int>(cycles[source].size());
            if (length != static_cast<int>(cycles[target].size())) {
                possible = false;
                continue;
            }

            int required_shift = (position[value] - position[i] + length) % length;

            if (target_of[source] != -1 &&
                (target_of[source] != target || shift[source] != required_shift)) {
                possible = false;
            }
            if (owner[target] != -1 && owner[target] != source) {
                possible = false;
            }

            if (target_of[source] == -1) {
                target_of[source] = target;
                shift[source] = required_shift;
            }
            if (owner[target] == -1) {
                owner[target] = source;
            }
        }

        if (possible) {
            vector<int> answer(n + 1, -1);
            vector<vector<int>> free_sources(n + 1), free_targets(n + 1);

            for (int id = 0; id < cycle_count; ++id) {
                int length = static_cast<int>(cycles[id].size());
                if (target_of[id] == -1) {
                    free_sources[length].push_back(id);
                } else {
                    const vector<int>& source_cycle = cycles[id];
                    const vector<int>& target_cycle = cycles[target_of[id]];
                    for (int j = 0; j < length; ++j) {
                        answer[source_cycle[j]] =
                            target_cycle[(j + shift[id]) % length];
                    }
                }

                if (owner[id] == -1) {
                    free_targets[length].push_back(id);
                }
            }

            for (int length = 1; length <= n; ++length) {
                auto by_first_vertex = [&](int lhs, int rhs) {
                    return cycles[lhs][0] < cycles[rhs][0];
                };
                sort(free_sources[length].begin(), free_sources[length].end(),
                     by_first_vertex);
                sort(free_targets[length].begin(), free_targets[length].end(),
                     by_first_vertex);

                if (free_sources[length].size() != free_targets[length].size()) {
                    possible = false;
                    break;
                }

                for (size_t k = 0; k < free_sources[length].size(); ++k) {
                    int source = free_sources[length][k];
                    int target = free_targets[length][k];
                    const vector<int>& source_cycle = cycles[source];
                    const vector<int>& target_cycle = cycles[target];

                    int free_shift =
                        (position[target_cycle[0]] - position[source_cycle[0]] + length) %
                        length;
                    for (int j = 0; j < length; ++j) {
                        answer[source_cycle[j]] =
                            target_cycle[(j + free_shift) % length];
                    }
                }
            }

            if (possible) {
                cout << "YES\n";
                for (int i = 1; i <= n; ++i) {
                    if (i > 1) {
                        cout << ' ';
                    }
                    cout << answer[i];
                }
                cout << '\n';
            }
        }

        if (!possible) {
            cout << "NO\n";
        }
    }

    return 0;
}
