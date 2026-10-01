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
        for (int i = 1; i <= n; ++i) cin >> a[i];
        for (int i = 1; i <= n; ++i) cin >> b[i];

        vector<vector<int>> cycles;
        vector<int> cycle_id(n + 1, -1), position(n + 1, -1);
        vector<char> seen(n + 1, false);

        for (int start = 1; start <= n; ++start) {
            if (seen[start]) continue;
            int id = static_cast<int>(cycles.size());
            vector<int> cycle;
            int u = start;
            while (!seen[u]) {
                seen[u] = true;
                cycle_id[u] = id;
                position[u] = static_cast<int>(cycle.size());
                cycle.push_back(u);
                u = a[u];
            }
            cycles.push_back(move(cycle));
        }

        int count = static_cast<int>(cycles.size());
        vector<int> mapped_cycle(count, -1), rotation(count, -1);
        vector<int> target_owner(count, -1);
        bool possible = true;

        for (int i = 1; i <= n && possible; ++i) {
            if (b[i] == -1) continue;

            int source = cycle_id[i];
            int target = cycle_id[b[i]];
            int length = static_cast<int>(cycles[source].size());
            if (length != static_cast<int>(cycles[target].size())) {
                possible = false;
                break;
            }

            int shift = (position[b[i]] - position[i] + length) % length;
            if (mapped_cycle[source] == -1) {
                if (target_owner[target] != -1 && target_owner[target] != source) {
                    possible = false;
                    break;
                }
                mapped_cycle[source] = target;
                rotation[source] = shift;
                target_owner[target] = source;
            } else if (mapped_cycle[source] != target || rotation[source] != shift) {
                possible = false;
            }
        }

        if (!possible) {
            cout << "NO\n";
            continue;
        }

        vector<int> answer(n + 1, 0);
        for (int source = 0; source < count; ++source) {
            if (mapped_cycle[source] == -1) continue;
            int target = mapped_cycle[source];
            int length = static_cast<int>(cycles[source].size());
            for (int x : cycles[source]) {
                int target_position = (position[x] + rotation[source]) % length;
                answer[x] = cycles[target][target_position];
            }
        }

        vector<int> minimum_vertex(count);
        vector<vector<pair<int, int>>> free_targets(n + 1);
        for (int id = 0; id < count; ++id) {
            minimum_vertex[id] = *min_element(cycles[id].begin(), cycles[id].end());
            if (target_owner[id] == -1) {
                int length = static_cast<int>(cycles[id].size());
                free_targets[length].push_back({minimum_vertex[id], id});
            }
        }
        for (auto &targets : free_targets) sort(targets.begin(), targets.end());

        vector<pair<int, int>> free_sources;
        for (int source = 0; source < count; ++source) {
            if (mapped_cycle[source] == -1) {
                free_sources.push_back({minimum_vertex[source], source});
            }
        }
        sort(free_sources.begin(), free_sources.end());

        vector<int> next_target(n + 1, 0);
        for (auto [first_index, source] : free_sources) {
            int length = static_cast<int>(cycles[source].size());
            if (next_target[length] == static_cast<int>(free_targets[length].size())) {
                possible = false;
                break;
            }

            int target = free_targets[length][next_target[length]++].second;
            int target_first = minimum_vertex[target];
            int shift = (position[target_first] - position[first_index] + length) % length;

            for (int x : cycles[source]) {
                int target_position = (position[x] + shift) % length;
                answer[x] = cycles[target][target_position];
            }
        }

        if (!possible) {
            cout << "NO\n";
            continue;
        }

        cout << "YES\n";
        for (int i = 1; i <= n; ++i) {
            if (i > 1) cout << ' ';
            cout << answer[i];
        }
        cout << '\n';
    }

    return 0;
}
