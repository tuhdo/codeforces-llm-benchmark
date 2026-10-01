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
        vector<int> a(n), b(n);
        for (int &x : a) { cin >> x; --x; }
        for (int &x : b) cin >> x;

        vector<int> cycle_id(n, -1), position(n, -1);
        vector<vector<int>> cycles;
        for (int start = 0; start < n; ++start) {
            if (cycle_id[start] != -1) continue;
            int id = (int)cycles.size();
            vector<int> cycle;
            int v = start;
            do {
                cycle_id[v] = id;
                position[v] = (int)cycle.size();
                cycle.push_back(v);
                v = a[v];
            } while (v != start);
            cycles.push_back(move(cycle));
        }

        int c = (int)cycles.size();
        vector<int> mapped_cycle(c, -1), shift(c, -1);
        bool possible = true;

        for (int i = 0; i < n; ++i) {
            if (b[i] == -1) continue;
            int target_vertex = b[i] - 1;
            int source = cycle_id[i];
            int target = cycle_id[target_vertex];
            int len = (int)cycles[source].size();
            if ((int)cycles[target].size() != len) {
                possible = false;
                break;
            }
            int delta = (position[target_vertex] - position[i] + len) % len;
            if (mapped_cycle[source] == -1) {
                mapped_cycle[source] = target;
                shift[source] = delta;
            } else if (mapped_cycle[source] != target || shift[source] != delta) {
                possible = false;
                break;
            }
        }

        vector<char> target_used(c, false);
        if (possible) {
            for (int source = 0; source < c; ++source) {
                if (mapped_cycle[source] == -1) continue;
                int target = mapped_cycle[source];
                if (target_used[target]) {
                    possible = false;
                    break;
                }
                target_used[target] = true;
            }
        }

        vector<int> answer(n, -1);
        if (possible) {
            for (int source = 0; source < c; ++source) {
                if (mapped_cycle[source] == -1) continue;
                const auto &from = cycles[source];
                const auto &to = cycles[mapped_cycle[source]];
                int len = (int)from.size();
                for (int k = 0; k < len; ++k) {
                    answer[from[k]] = to[(k + shift[source]) % len] + 1;
                }
            }

            map<int, vector<int>> sources_by_length, targets_by_length;
            for (int id = 0; id < c; ++id) {
                int len = (int)cycles[id].size();
                if (mapped_cycle[id] == -1) sources_by_length[len].push_back(id);
                if (!target_used[id]) targets_by_length[len].push_back(id);
            }

            for (auto &[len, sources] : sources_by_length) {
                auto &targets = targets_by_length[len];
                auto by_min_vertex = [&](int x, int y) {
                    return *min_element(cycles[x].begin(), cycles[x].end()) <
                           *min_element(cycles[y].begin(), cycles[y].end());
                };
                sort(sources.begin(), sources.end(), by_min_vertex);
                sort(targets.begin(), targets.end(), by_min_vertex);
                if (sources.size() != targets.size()) {
                    possible = false;
                    break;
                }
                for (int j = 0; j < (int)sources.size(); ++j) {
                    int source = sources[j], target = targets[j];
                    int anchor_source = *min_element(cycles[source].begin(), cycles[source].end());
                    int anchor_target = *min_element(cycles[target].begin(), cycles[target].end());
                    int delta = (position[anchor_target] - position[anchor_source] + len) % len;
                    const auto &from = cycles[source];
                    const auto &to = cycles[target];
                    for (int k = 0; k < len; ++k) {
                        answer[from[k]] = to[(k + delta) % len] + 1;
                    }
                }
            }
        }

        if (!possible) {
            cout << "NO\n";
        } else {
            cout << "YES\n";
            for (int i = 0; i < n; ++i) {
                if (i) cout << ' ';
                cout << answer[i];
            }
            cout << '\n';
        }
    }
    return 0;
}
