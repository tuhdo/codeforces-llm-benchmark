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

        vector<int> a(n + 1);
        for (int i = 1; i <= n; ++i) cin >> a[i];

        vector<vector<int>> cycles;
        vector<int> cycle_id(n + 1, -1), position(n + 1, -1);
        vector<char> visited(n + 1, false);

        for (int start = 1; start <= n; ++start) {
            if (visited[start]) continue;
            vector<int> cycle;
            int v = start;
            do {
                visited[v] = true;
                cycle_id[v] = (int)cycles.size();
                position[v] = (int)cycle.size();
                cycle.push_back(v);
                v = a[v];
            } while (v != start);
            cycles.push_back(move(cycle));
        }

        const int cycle_count = (int)cycles.size();
        vector<int> b(n + 1);
        for (int i = 1; i <= n; ++i) cin >> b[i];

        vector<int> fixed_destination(cycle_count, -1);
        vector<int> fixed_shift(cycle_count, 0);
        bool possible = true;

        for (int i = 1; i <= n; ++i) {
            if (b[i] == -1) continue;

            int source = cycle_id[i];
            int destination = cycle_id[b[i]];
            int length = (int)cycles[source].size();
            if ((int)cycles[destination].size() != length) {
                possible = false;
                continue;
            }

            int shift = position[b[i]] - position[i];
            shift %= length;
            if (shift < 0) shift += length;

            if (fixed_destination[source] == -1) {
                fixed_destination[source] = destination;
                fixed_shift[source] = shift;
            } else if (fixed_destination[source] != destination ||
                       fixed_shift[source] != shift) {
                possible = false;
            }
        }

        vector<char> destination_used(cycle_count, false);
        for (int source = 0; source < cycle_count; ++source) {
            if (fixed_destination[source] == -1) continue;
            int destination = fixed_destination[source];
            if (destination_used[destination]) {
                possible = false;
            } else {
                destination_used[destination] = true;
            }
        }

        if (!possible) {
            cout << "NO\n";
            continue;
        }

        vector<int> answer(n + 1, -1);
        for (int source = 0; source < cycle_count; ++source) {
            if (fixed_destination[source] == -1) continue;
            const auto& from = cycles[source];
            const auto& to = cycles[fixed_destination[source]];
            int shift = fixed_shift[source];
            for (int k = 0; k < (int)from.size(); ++k) {
                answer[from[k]] = to[(k + shift) % from.size()];
            }
        }

        vector<set<int>> available(n + 1);
        for (int destination = 0; destination < cycle_count; ++destination) {
            if (destination_used[destination]) continue;
            int length = (int)cycles[destination].size();
            for (int value : cycles[destination]) available[length].insert(value);
        }

        vector<int> source_order(cycle_count);
        iota(source_order.begin(), source_order.end(), 0);
        sort(source_order.begin(), source_order.end(), [&](int x, int y) {
            return *min_element(cycles[x].begin(), cycles[x].end()) <
                   *min_element(cycles[y].begin(), cycles[y].end());
        });

        for (int source : source_order) {
            if (fixed_destination[source] != -1) continue;

            const auto& from = cycles[source];
            int length = (int)from.size();
            if (available[length].empty()) {
                possible = false;
                break;
            }

            int first = *min_element(from.begin(), from.end());
            int target_value = *available[length].begin();
            int destination = cycle_id[target_value];
            const auto& to = cycles[destination];

            int shift = position[target_value] - position[first];
            shift %= length;
            if (shift < 0) shift += length;

            for (int k = 0; k < length; ++k) {
                answer[from[k]] = to[(k + shift) % length];
            }
            for (int value : to) available[length].erase(value);
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
