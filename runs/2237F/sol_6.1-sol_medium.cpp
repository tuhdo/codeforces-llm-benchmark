#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int tests;
    std::cin >> tests;
    while (tests--) {
        int n, m;
        std::cin >> n >> m;

        const int starts = n - m + 1;
        std::vector<int> best(starts + 1, 0);
        std::vector<int> prefix(n + 1, 0);
        int finished = 0;

        for (int i = 1; i <= n; ++i) {
            int value;
            std::cin >> value;

            // This interval has ended, so its best value cannot change again.
            if (i > m) {
                finished = std::max(finished, best[i - m]);
            }

            const int start = i - value + 1;
            int current = 0;
            if (1 <= start && start <= starts) {
                current = 1 + std::max({best[start], prefix[start - 1], finished});
                best[start] = std::max(best[start], current);
            }
            prefix[i] = std::max(prefix[i - 1], current);
        }

        std::cout << n - prefix[n] << '\n';
    }
}
