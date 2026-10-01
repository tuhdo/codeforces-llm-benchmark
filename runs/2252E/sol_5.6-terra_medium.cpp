#include <array>
#include <iostream>

using namespace std;

constexpr long long MOD = 1'000'000'007LL;
constexpr long long INV2 = (MOD + 1) / 2;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        unsigned long long n;
        cin >> n;

        // State: previous bit of h, and the borrows in n-a and n-c.
        long long dp[2][2][2]{};
        dp[0][0][0] = 1;

        for (int bit = 0; bit <= 62; ++bit) {
            long long next[2][2][2]{};
            const int nbit = static_cast<int>((n >> bit) & 1ULL);

            for (int previous = 0; previous <= 1; ++previous) {
                for (int borrowA = 0; borrowA <= 1; ++borrowA) {
                    for (int borrowC = 0; borrowC <= 1; ++borrowC) {
                        const long long ways = dp[previous][borrowA][borrowC];
                        if (ways == 0) continue;

                        // The final zero bit flushes the dependency on the
                        // preceding bit of h. Earlier bits may choose h_bit.
                        const int maxHBit = (bit == 62 ? 0 : 1);
                        for (int hbit = 0; hbit <= maxHBit; ++hbit) {
                            if (previous && hbit) continue; // h has no adjacent ones

                            array<pair<int, int>, 2> options{};
                            int count = 0;
                            if (hbit) {
                                options[count++] = {1, 1};
                            } else if (previous) {
                                options[count++] = {0, 1};
                                options[count++] = {1, 0};
                            } else {
                                options[count++] = {0, 0};
                            }

                            for (int choice = 0; choice < count; ++choice) {
                                const auto [abit, cbit] = options[choice];
                                const int nextBorrowA = nbit < abit + borrowA;
                                const int nextBorrowC = nbit < cbit + borrowC;
                                long long& target = next[hbit][nextBorrowA][nextBorrowC];
                                target += ways;
                                if (target >= MOD) target -= MOD;
                            }
                        }
                    }
                }
            }
            for (int h = 0; h <= 1; ++h)
                for (int a = 0; a <= 1; ++a)
                    for (int c = 0; c <= 1; ++c)
                        dp[h][a][c] = next[h][a][c];
        }

        // Each nonzero h yields two ordered orientations of (a, c).
        // The lone h=0 path represents (0, 0), which is not a triplet.
        long long ordered = dp[0][0][0] - 1;
        if (ordered < 0) ordered += MOD;
        cout << ordered * INV2 % MOD << '\n';
    }
}
