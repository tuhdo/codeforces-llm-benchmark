#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int testCount;
    cin >> testCount;

    while (testCount--) {
        int n;
        cin >> n;

        vector<int> a(n + 1);
        vector<int> given(n + 1);
        for (int i = 1; i <= n; ++i) {
            cin >> a[i];
        }
        for (int i = 1; i <= n; ++i) {
            cin >> given[i];
        }

        vector<int> cycleId(n + 1, -1);
        vector<int> positionInCycle(n + 1, -1);
        vector<bool> visited(n + 1, false);
        vector<vector<int>> cycles;

        for (int start = 1; start <= n; ++start) {
            if (visited[start]) {
                continue;
            }

            int id = static_cast<int>(cycles.size());
            vector<int> cycle;
            for (int x = start; !visited[x]; x = a[x]) {
                visited[x] = true;
                cycleId[x] = id;
                positionInCycle[x] = static_cast<int>(cycle.size());
                cycle.push_back(x);
            }
            cycles.push_back(move(cycle));
        }

        int cycleCount = static_cast<int>(cycles.size());
        vector<int> targetCycle(cycleCount, -1);
        vector<int> rotation(cycleCount, 0);
        vector<int> targetOwner(cycleCount, -1);
        bool possible = true;

        // A known value fixes both the target cycle and the rotation of its
        // whole source cycle.
        for (int i = 1; i <= n; ++i) {
            if (given[i] == -1) {
                continue;
            }

            int source = cycleId[i];
            int target = cycleId[given[i]];
            int length = static_cast<int>(cycles[source].size());

            if (length != static_cast<int>(cycles[target].size())) {
                possible = false;
                continue;
            }

            int neededRotation = positionInCycle[given[i]] - positionInCycle[i];
            if (neededRotation < 0) {
                neededRotation += length;
            }

            if (targetCycle[source] == -1) {
                targetCycle[source] = target;
                rotation[source] = neededRotation;
            } else if (targetCycle[source] != target || rotation[source] != neededRotation) {
                possible = false;
            }
        }

        // Different source cycles must have different target cycles, since b
        // itself has to be a permutation.
        for (int source = 0; source < cycleCount; ++source) {
            if (targetCycle[source] == -1) {
                continue;
            }

            int target = targetCycle[source];
            if (targetOwner[target] != -1 && targetOwner[target] != source) {
                possible = false;
            } else {
                targetOwner[target] = source;
            }
        }

        vector<int> minimumElement(cycleCount);
        for (int id = 0; id < cycleCount; ++id) {
            minimumElement[id] = *min_element(cycles[id].begin(), cycles[id].end());
        }

        if (possible) {
            map<int, vector<int>> freeSources;
            map<int, vector<int>> freeTargets;

            for (int id = 0; id < cycleCount; ++id) {
                int length = static_cast<int>(cycles[id].size());
                if (targetCycle[id] == -1) {
                    freeSources[length].push_back(id);
                }
                if (targetOwner[id] == -1) {
                    freeTargets[length].push_back(id);
                }
            }

            for (auto& [length, sources] : freeSources) {
                vector<int>& targets = freeTargets[length];
                if (sources.size() != targets.size()) {
                    possible = false;
                    break;
                }

                auto byMinimumElement = [&](int left, int right) {
                    return minimumElement[left] < minimumElement[right];
                };
                sort(sources.begin(), sources.end(), byMinimumElement);
                sort(targets.begin(), targets.end(), byMinimumElement);

                for (size_t i = 0; i < sources.size(); ++i) {
                    int source = sources[i];
                    int target = targets[i];
                    int sourceMinimum = minimumElement[source];
                    int targetMinimum = minimumElement[target];
                    int shift = positionInCycle[targetMinimum] - positionInCycle[sourceMinimum];
                    if (shift < 0) {
                        shift += length;
                    }

                    targetCycle[source] = target;
                    rotation[source] = shift;
                }
            }
        }

        if (!possible) {
            cout << "NO\n";
            continue;
        }

        vector<int> answer(n + 1);
        for (int source = 0; source < cycleCount; ++source) {
            int target = targetCycle[source];
            int length = static_cast<int>(cycles[source].size());

            for (int i = 0; i < length; ++i) {
                answer[cycles[source][i]] = cycles[target][(i + rotation[source]) % length];
            }
        }

        cout << "YES\n";
        for (int i = 1; i <= n; ++i) {
            cout << answer[i] << " \n"[i == n];
        }
    }

    return 0;
}
