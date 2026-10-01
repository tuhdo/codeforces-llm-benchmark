#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;

    while (tests--) {
        int n;
        cin >> n;

        vector<int> a(n + 1), given(n + 1);
        for (int i = 1; i <= n; ++i) cin >> a[i];
        for (int i = 1; i <= n; ++i) cin >> given[i];

        vector<vector<int>> cycles;
        vector<int> cycleOf(n + 1), positionInCycle(n + 1);
        vector<bool> seen(n + 1, false);

        for (int start = 1; start <= n; ++start) {
            if (seen[start]) continue;

            int id = static_cast<int>(cycles.size());
            vector<int> cycle;
            int v = start;
            while (!seen[v]) {
                seen[v] = true;
                cycleOf[v] = id;
                positionInCycle[v] = static_cast<int>(cycle.size());
                cycle.push_back(v);
                v = a[v];
            }
            cycles.push_back(move(cycle));
        }

        int countCycles = static_cast<int>(cycles.size());
        vector<int> cycleLength(countCycles), minimumValue(countCycles);
        for (int c = 0; c < countCycles; ++c) {
            cycleLength[c] = static_cast<int>(cycles[c].size());
            minimumValue[c] = *min_element(cycles[c].begin(), cycles[c].end());
        }

        vector<int> targetOfSource(countCycles, -1);
        vector<int> rotation(countCycles, 0);
        vector<int> sourceOfTarget(countCycles, -1);
        bool possible = true;

        for (int i = 1; i <= n; ++i) {
            if (given[i] == -1) continue;

            int source = cycleOf[i];
            int target = cycleOf[given[i]];
            if (cycleLength[source] != cycleLength[target]) {
                possible = false;
                break;
            }

            int length = cycleLength[source];
            int requiredRotation = (positionInCycle[given[i]] - positionInCycle[i] + length) % length;

            if (targetOfSource[source] != -1 &&
                (targetOfSource[source] != target || rotation[source] != requiredRotation)) {
                possible = false;
                break;
            }

            targetOfSource[source] = target;
            rotation[source] = requiredRotation;

            if (sourceOfTarget[target] != -1 && sourceOfTarget[target] != source) {
                possible = false;
                break;
            }
            sourceOfTarget[target] = source;
        }

        if (!possible) {
            cout << "NO\n";
            continue;
        }

        vector<priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>>> candidates(n + 1);
        for (int c = 0; c < countCycles; ++c) {
            candidates[cycleLength[c]].push({minimumValue[c], c});
        }

        vector<bool> targetUsed(countCycles, false);
        for (int target = 0; target < countCycles; ++target) {
            if (sourceOfTarget[target] != -1) targetUsed[target] = true;
        }

        vector<int> answer(n + 1);
        for (int i = 1; i <= n; ++i) {
            int source = cycleOf[i];

            if (targetOfSource[source] == -1) {
                int length = cycleLength[source];
                auto &heap = candidates[length];
                while (!heap.empty() && targetUsed[heap.top().second]) heap.pop();

                if (heap.empty()) {
                    possible = false;
                    break;
                }

                auto [value, target] = heap.top();
                heap.pop();
                targetUsed[target] = true;
                targetOfSource[source] = target;
                rotation[source] = (positionInCycle[value] - positionInCycle[i] + length) % length;
            }

            int target = targetOfSource[source];
            int length = cycleLength[source];
            answer[i] = cycles[target][(positionInCycle[i] + rotation[source]) % length];
        }

        if (!possible) {
            cout << "NO\n";
            continue;
        }

        cout << "YES\n";
        for (int i = 1; i <= n; ++i) {
            cout << answer[i] << " \n"[i == n];
        }
    }

    return 0;
}
