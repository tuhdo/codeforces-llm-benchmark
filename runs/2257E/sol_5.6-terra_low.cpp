#include <bits/stdc++.h>
using namespace std;

struct Project {
    vector<long long> a, b;
    int pos = 0;
};

struct Block {
    long long need, gain;
    int project, end;
    bool operator>(const Block& other) const { return need > other.need; }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;
    while (T--) {
        int n;
        long long money;
        cin >> n >> money;
        vector<Project> projects(n);
        for (auto& p : projects) {
            int m;
            cin >> m;
            p.a.resize(m);
            p.b.resize(m);
            for (long long& value : p.a) cin >> value;
            for (long long& value : p.b) cin >> value;
        }

        priority_queue<Block, vector<Block>, greater<Block>> available;

        // Add the shortest following segment with nonnegative total profit.
        // Its internal order is fixed, so `need` is the capital required to
        // execute the whole segment consecutively.
        auto add_block = [&](int id) {
            Project& p = projects[id];
            long long balance = 0;
            long long need = 0;
            for (int j = p.pos; j < (int)p.a.size(); ++j) {
                need = max(need, p.a[j] - balance);
                balance += p.b[j] - p.a[j];
                if (balance >= 0) {
                    available.push({need, balance, id, j + 1});
                    return;
                }
            }
        };

        for (int i = 0; i < n; ++i) add_block(i);

        while (!available.empty() && available.top().need <= money) {
            Block cur = available.top();
            available.pop();
            // This project cannot have another outstanding block: blocks are
            // inserted only after their predecessor has been completed.
            projects[cur.project].pos = cur.end;
            money += cur.gain;
            add_block(cur.project);
        }

        int best_height = -1;
        int best_project = -1;
        for (int i = 0; i < n; ++i) {
            long long current = money;
            int j = projects[i].pos;
            while (j < (int)projects[i].a.size() && current >= projects[i].a[j]) {
                current += projects[i].b[j] - projects[i].a[j];
                ++j;
            }
            if (j > best_height) {
                best_height = j;
                best_project = i;
            }
        }
        cout << best_height << ' ' << best_project + 1 << '\n';
    }
}
