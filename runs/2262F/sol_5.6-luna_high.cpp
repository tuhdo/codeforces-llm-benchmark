#include <bits/stdc++.h>
using namespace std;

constexpr int MAXN = 305;
using Bits = bitset<MAXN>;

struct HopcroftKarp {
    int n;
    const vector<vector<int>>& graph;
    vector<int> left_match, right_match, dist;

    HopcroftKarp(const vector<vector<int>>& graph)
        : n(static_cast<int>(graph.size())), graph(graph),
          left_match(n, -1), right_match(n, -1), dist(n) {}

    bool bfs() {
        queue<int> q;
        for (int u = 0; u < n; ++u) {
            if (left_match[u] == -1) {
                dist[u] = 0;
                q.push(u);
            } else {
                dist[u] = -1;
            }
        }

        bool found_augmenting_path = false;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (int v : graph[u]) {
                int next = right_match[v];
                if (next == -1) {
                    found_augmenting_path = true;
                } else if (dist[next] == -1) {
                    dist[next] = dist[u] + 1;
                    q.push(next);
                }
            }
        }
        return found_augmenting_path;
    }

    bool dfs(int u) {
        for (int v : graph[u]) {
            int next = right_match[v];
            if (next == -1 || (dist[next] == dist[u] + 1 && dfs(next))) {
                left_match[u] = v;
                right_match[v] = u;
                return true;
            }
        }
        dist[u] = -1;
        return false;
    }

    vector<int> run() {
        while (bfs()) {
            for (int u = 0; u < n; ++u) {
                if (left_match[u] == -1) {
                    dfs(u);
                }
            }
        }
        return left_match;
    }
};

int gf2_rank(const vector<Bits>& matrix, int n) {
    vector<Bits> a = matrix;
    int rank = 0;
    for (int col = 0; col < n; ++col) {
        int pivot = -1;
        for (int row = rank; row < n; ++row) {
            if (a[row][col]) {
                pivot = row;
                break;
            }
        }
        if (pivot == -1) {
            continue;
        }
        swap(a[rank], a[pivot]);
        for (int row = 0; row < n; ++row) {
            if (row != rank && a[row][col]) {
                a[row] ^= a[rank];
            }
        }
        ++rank;
    }
    return rank;
}

vector<int> find_matching(const vector<pair<int, int>>& cells, int n) {
    vector<vector<int>> graph(n);
    for (auto [row, col] : cells) {
        graph[row].push_back(col);
    }
    return HopcroftKarp(graph).run();
}

void rebuild_cells(const vector<Bits>& matrix, int n, vector<pair<int, int>>& cells) {
    cells.clear();
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            if (matrix[row][col]) {
                cells.emplace_back(row, col);
            }
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;
    while (tests--) {
        int n, m;
        cin >> n >> m;

        vector<pair<int, int>> cells;
        cells.reserve(m);
        vector<Bits> matrix(n);
        for (int i = 0; i < m; ++i) {
            int x, y;
            cin >> x >> y;
            --x;
            --y;
            cells.emplace_back(x, y);
            matrix[x][y] = true;
        }

        int moves = (m + n - 1) / n;
        vector<vector<pair<int, int>>> answer;
        answer.reserve(moves);

        if (moves == 1) {
            answer.push_back(cells);
        } else {
            int remainder = m % n;
            if (remainder == 0) {
                remainder = n;
            }

            mt19937_64 rng(
                chrono::steady_clock::now().time_since_epoch().count() ^
                (static_cast<uint64_t>(n) << 32) ^ static_cast<uint64_t>(m));

            int full_rank_moves = moves - 2;
            for (int step = 0; step < full_rank_moves; ++step) {
                int current_count = static_cast<int>(cells.size());
                vector<int> matching = find_matching(cells, n);
                vector<int> owner(n);
                for (int row = 0; row < n; ++row) {
                    owner[matching[row]] = row;
                }

                vector<pair<int, int>> removed;
                vector<Bits> next_matrix = matrix;

                if (current_count <= 3 * n) {
                    // Keep a matching and choose extra edges that point forward.
                    // The kept graph then has a unique perfect matching.
                    vector<Bits> keep(n);
                    vector<pair<int, int>> extra;
                    extra.reserve(current_count - n);
                    for (auto [row, col] : cells) {
                        if (matching[row] == col) {
                            keep[row][col] = true;
                        } else {
                            extra.emplace_back(row, col);
                        }
                    }

                    int need_extra = current_count - 2 * n;
                    int forward_normal = 0;
                    int forward_reverse = 0;
                    for (auto [row, col] : extra) {
                        int to = owner[col];
                        forward_normal += row < to;
                        forward_reverse += row > to;
                    }
                    bool normal = forward_normal >= forward_reverse;
                    int chosen = 0;
                    for (auto [row, col] : extra) {
                        int to = owner[col];
                        bool forward = normal ? (row < to) : (row > to);
                        if (forward && chosen < need_extra) {
                            keep[row][col] = true;
                            ++chosen;
                        }
                    }

                    for (auto [row, col] : cells) {
                        if (!keep[row][col]) {
                            removed.emplace_back(row, col);
                            next_matrix[row][col] = false;
                        }
                    }
                } else {
                    // In the dense case, sample a deletion set and verify it.
                    // The check is one-sided: an unsuccessful sample is discarded.
                    vector<pair<int, int>> extra;
                    extra.reserve(current_count - n);
                    for (auto [row, col] : cells) {
                        if (matching[row] != col) {
                            extra.emplace_back(row, col);
                        }
                    }
                    uint64_t attempts = 0;
                    while (true) {
                        const auto& pool = attempts < 1000 ? extra : cells;
                        int pool_count = static_cast<int>(pool.size());
                        vector<int> order(pool_count);
                        iota(order.begin(), order.end(), 0);
                        for (int i = 0; i < n; ++i) {
                            uniform_int_distribution<int> pick(i, pool_count - 1);
                            swap(order[i], order[pick(rng)]);
                        }

                        next_matrix = matrix;
                        for (int i = 0; i < n; ++i) {
                            auto [row, col] = pool[order[i]];
                            next_matrix[row][col] = false;
                        }
                        if (gf2_rank(next_matrix, n) == n) {
                            for (int i = 0; i < n; ++i) {
                                removed.push_back(pool[order[i]]);
                            }
                            break;
                        }
                        ++attempts;
                    }
                }

                // The construction or rank check gives exactly n deletions and
                // leaves a full-rank matrix for the next move.
                answer.push_back(removed);
                matrix.swap(next_matrix);
                rebuild_cells(matrix, n, cells);
            }

            vector<int> matching = find_matching(cells, n);
            vector<Bits> keep(n);
            vector<pair<int, int>> last;
            last.reserve(remainder);
            for (int row = 0; row < remainder; ++row) {
                keep[row][matching[row]] = true;
                last.emplace_back(row, matching[row]);
            }

            vector<pair<int, int>> penultimate;
            penultimate.reserve(n);
            for (auto [row, col] : cells) {
                if (!keep[row][col]) {
                    penultimate.emplace_back(row, col);
                }
            }
            answer.push_back(penultimate);
            answer.push_back(last);
        }

        cout << answer.size() << '\n';
        for (const auto& move : answer) {
            cout << move.size() << '\n';
            for (auto [x, y] : move) {
                cout << x + 1 << ' ' << y + 1 << '\n';
            }
        }
    }
    return 0;
}
