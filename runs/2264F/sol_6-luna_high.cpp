#include <bits/stdc++.h>
using namespace std;

string ones(int count) {
    string s = "n";
    for (int i = 1; i < count; ++i) s += "+n";
    return s;
}

string constant(int value) {
    if (value == 1) return "n/n";
    return "(" + ones(value) + ")/n";
}

string atLeast(int value) {
    return "round(n/(" + constant(value) + "+n))";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int k;
    cin >> k;

    const string one = "n/n";

    // For an integer n, round(n / (n + value)) is [n >= value].
    // Thus this product evaluates to n! for every 2 <= n <= k.
    string factorial = one;
    for (int j = 2; j <= k; ++j) {
        factorial += "*(" + atLeast(j) + "*(" + constant(j - 1) + ")+" + one + ")";
    }

    // Build 1 + 1/1! + ... + 1/k! as a nested expression:
    // e_k = 1 + (1/1) * (1 + (1/2) * (... (1/k) ...)).
    string eApprox = one;
    for (int i = k; i >= 1; --i) {
        eApprox = one + "+n/(" + ones(i) + ")*(" + eApprox + ")";
    }

    cout << "round((" << factorial << ")/(" << eApprox << "))\n";
}
