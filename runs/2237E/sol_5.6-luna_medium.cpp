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
        for (int &x : a) cin >> x, --x;
        for (int &x : b) {
            cin >> x;
            if (x != -1) --x;
        }

        vector<int> cycleId(n, -1), position(n, -1);
        vector<vector<int>> cycles;
        for (int i = 0; i < n; ++i) {
            if (cycleId[i] != -1) continue;
            int id = (int)cycles.size();
            vector<int> cyc;
            int v = i;
            while (cycleId[v] == -1) {
                cycleId[v] = id;
                position[v] = (int)cyc.size();
                cyc.push_back(v);
                v = a[v];
            }
            cycles.push_back(move(cyc));
        }

        int m = (int)cycles.size();
        vector<int> targetOf(m, -1), shiftOf(m, 0);
        bool ok = true;

        for (int i = 0; i < n; ++i) {
            if (b[i] == -1) continue;
            int source = cycleId[i];
            int target = cycleId[b[i]];
            int len = (int)cycles[source].size();
            if ((int)cycles[target].size() != len) {
                ok = false;
                continue;
            }
            int shift = position[b[i]] - position[i];
            shift %= len;
            if (shift < 0) shift += len;
            if (targetOf[source] == -1) {
                targetOf[source] = target;
                shiftOf[source] = shift;
            } else if (targetOf[source] != target || shiftOf[source] != shift) {
                ok = false;
            }
        }

        vector<int> sourceOf(m, -1);
        for (int source = 0; source < m; ++source) {
            if (targetOf[source] == -1) continue;
            int target = targetOf[source];
            if (sourceOf[target] != -1 && sourceOf[target] != source) {
                ok = false;
            } else {
                sourceOf[target] = source;
            }
        }

        vector<vector<int>> byLength(n + 1);
        for (int id = 0; id < m; ++id) {
            byLength[cycles[id].size()].push_back(id);
        }

        for (int len = 1; len <= n; ++len) {
            auto &ids = byLength[len];
            vector<int> freeSources, freeTargets;
            for (int id : ids) {
                if (targetOf[id] == -1) freeSources.push_back(id);
                if (sourceOf[id] == -1) freeTargets.push_back(id);
            }

            auto firstVertex = [&](int id) { return cycles[id][0]; };
            sort(freeSources.begin(), freeSources.end(),
                 [&](int x, int y) { return firstVertex(x) < firstVertex(y); });
            sort(freeTargets.begin(), freeTargets.end(),
                 [&](int x, int y) { return firstVertex(x) < firstVertex(y); });

            if (freeSources.size() != freeTargets.size()) {
                ok = false;
                continue;
            }
            for (int k = 0; k < (int)freeSources.size(); ++k) {
                int source = freeSources[k];
                int target = freeTargets[k];
                targetOf[source] = target;

                int sourceFirst = cycles[source][0];
                int targetFirst = cycles[target][0];
                int lenNow = (int)cycles[source].size();
                shiftOf[source] = position[targetFirst] - position[sourceFirst];
                shiftOf[source] %= lenNow;
                if (shiftOf[source] < 0) shiftOf[source] += lenNow;
            }
        }

        if (!ok) {
            cout << "NO\n";
            continue;
        }

        vector<int> answer(n);
        for (int source = 0; source < m; ++source) {
            int target = targetOf[source];
            int len = (int)cycles[source].size();
            for (int p = 0; p < len; ++p) {
                int from = cycles[source][p];
                int to = cycles[target][(p + shiftOf[source]) % len];
                answer[from] = to;
            }
        }

        cout << "YES\n";
        for (int i = 0; i < n; ++i) {
            if (i) cout << ' ';
            cout << answer[i] + 1;
        }
        cout << '\n';
    }
}
