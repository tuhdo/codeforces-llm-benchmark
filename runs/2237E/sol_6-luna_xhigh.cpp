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

        vector<int> cycle_id(n + 1, -1), position(n + 1, -1);
        vector<vector<int>> cycles;
        for (int start = 1; start <= n; ++start) {
            if (cycle_id[start] != -1) continue;

            int id = static_cast<int>(cycles.size());
            vector<int> cycle;
            int v = start;
            do {
                cycle_id[v] = id;
                position[v] = static_cast<int>(cycle.size());
                cycle.push_back(v);
                v = a[v];
            } while (v != start);
            cycles.push_back(move(cycle));
        }

        const int cycle_count = static_cast<int>(cycles.size());
        vector<int> mapped_cycle(cycle_count, -1);
        vector<int> mapped_shift(cycle_count, -1);
        bool possible = true;

        for (int i = 1; i <= n; ++i) {
            if (b[i] == -1) continue;

            int source = cycle_id[i];
            int target = cycle_id[b[i]];
            int length = static_cast<int>(cycles[source].size());
            if (length != static_cast<int>(cycles[target].size())) {
                possible = false;
                continue;
            }

            int shift = (position[b[i]] - position[i] + length) % length;
            if (mapped_cycle[source] == -1) {
                mapped_cycle[source] = target;
                mapped_shift[source] = shift;
            } else if (mapped_cycle[source] != target || mapped_shift[source] != shift) {
                possible = false;
            }
        }

        vector<int> target_owner(cycle_count, -1);
        for (int source = 0; source < cycle_count; ++source) {
            int target = mapped_cycle[source];
            if (target == -1) continue;
            if (target_owner[target] != -1) {
                possible = false;
            } else {
                target_owner[target] = source;
            }
        }

        if (!possible) {
            cout << "NO\n";
            continue;
        }

        vector<int> answer(n + 1, -1);
        for (int source = 0; source < cycle_count; ++source) {
            int target = mapped_cycle[source];
            if (target == -1) continue;

            int length = static_cast<int>(cycles[source].size());
            int shift = mapped_shift[source];
            for (int j = 0; j < length; ++j) {
                answer[cycles[source][j]] = cycles[target][(j + shift) % length];
            }
        }

        vector<vector<int>> available_targets(n + 1);
        for (int target = 0; target < cycle_count; ++target) {
            if (target_owner[target] == -1) {
                int length = static_cast<int>(cycles[target].size());
                available_targets[length].push_back(target);
            }
        }

        vector<size_t> next_target(n + 1, 0);
        for (int source = 0; source < cycle_count; ++source) {
            if (mapped_cycle[source] != -1) continue;

            int length = static_cast<int>(cycles[source].size());
            size_t &at = next_target[length];
            if (at == available_targets[length].size()) {
                possible = false;
                break;
            }

            int target = available_targets[length][at++];
            // Cycles are stored starting at their smallest element. Mapping
            // the two starts to each other minimizes the first free value.
            for (int j = 0; j < length; ++j) {
                answer[cycles[source][j]] = cycles[target][j];
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
