#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

struct Type {
    int64 value;
    int64 count;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tests;
    cin >> tests;

    while (tests--) {
        int n;
        cin >> n;

        vector<Type> types(n);
        int64 total = 0;
        int64 largest = 0;
        for (Type &type : types) {
            cin >> type.value >> type.count;
            total += type.count;
            largest = max(largest, type.value);
        }

        sort(types.begin(), types.end(), [](const Type &a, const Type &b) {
            return a.value > b.value;
        });

        // This function is only called for target > largest, so the target
        // itself is absent and must be formed by one MEX operation.
        auto canCreate = [&](int64 target) {
            int64 requiredAtLowerLevels = 1;
            int64 consumedMatchingValues = 0;
            int64 previous = target;

            auto passAbsentLevels = [&](int64 levels) {
                while (levels-- > 0) {
                    // An absent value must be built, doubling the number of
                    // required nodes at every lower value.
                    if (requiredAtLowerLevels > total / 2) {
                        return false;
                    }
                    requiredAtLowerLevels *= 2;
                }
                return true;
            };

            for (const Type &type : types) {
                if (type.value < 2) {
                    break;
                }

                const int64 absentBetween = previous - type.value - 1;
                if (!passAbsentLevels(absentBetween)) {
                    return false;
                }

                // At this value, use every available original occurrence
                // before expanding a node into all smaller values.
                const int64 used = min(requiredAtLowerLevels, type.count);
                consumedMatchingValues += used;
                requiredAtLowerLevels = 2 * requiredAtLowerLevels - used;
                if (requiredAtLowerLevels > total / 2) {
                    return false;
                }

                previous = type.value;
            }

            // Values from previous - 1 down through 2 are absent.
            if (!passAbsentLevels(previous - 2)) {
                return false;
            }

            // Every still-unmatched 0 or 1 node can be made from one unused
            // original element. Any original value can be changed into 0 or 1.
            return 2 * requiredAtLowerLevels <= total - consumedMatchingValues;
        };

        int64 answer = largest;
        while (canCreate(answer + 1)) {
            ++answer;
        }

        cout << answer << '\n';
    }
}
