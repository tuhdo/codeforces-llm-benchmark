#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int k;
    cin >> k;

    // Each constant expression has the same integer value for every n > 0.
    vector<string> constant(k + 1);
    constant[1] = "(n/n)";
    for (int value = 2; value <= k; ++value) {
        string sum = "n";
        for (int j = 1; j < value; ++j) sum += "+n";
        constant[value] = "((" + sum + ")/n)";

        auto consider = [&](const string& candidate) {
            if (candidate.size() < constant[value].size()) {
                constant[value] = candidate;
            }
        };
        for (int left = 1; left < value; ++left) {
            consider("(" + constant[left] + "+" + constant[value - left] + ")");
            if (left > 1 && value % left == 0) {
                consider("(" + constant[left] + "*" + constant[value / left] + ")");
            }
        }
    }

    string factorial;
    for (int i = 2; i <= k; ++i) {
        if (i > 2) factorial += "*";
        factorial += "(" + constant[1] + "+" + constant[i - 1]
                   + "*round(n/(n+" + constant[i] + ")))";
    }

    // 1/2 * (1 - 1/3 * (1 - 1/4 * (...))) = sum_{j=0}^k (-1)^j/j!.
    string tail = constant[1];
    for (int i = k; i >= 3; --i) {
        tail = "(" + constant[1] + "-" + tail + "/" + constant[i] + ")";
    }
    string coefficient = "(" + tail + "/" + constant[2] + ")";
    cout << "round(" << factorial << "*" << coefficient << ")\n";
}
