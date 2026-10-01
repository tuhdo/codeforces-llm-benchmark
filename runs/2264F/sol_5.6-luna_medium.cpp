#include <bits/stdc++.h>
using namespace std;

// Build a positive integer using only n and arithmetic operations.
static string make_constant(int value) {
    if (value == 0) return "(n-n)";
    if (value == 1) return "n/n";
    if (value == 2) return "(n+n)";

    string bits;
    for (int x = value; x; x >>= 1) bits.push_back(char('0' + (x & 1)));
    reverse(bits.begin(), bits.end());

    string result = "n/n";
    for (size_t i = 1; i < bits.size(); ++i) {
        result = "(" + result + "*(n+n))";
        if (bits[i] == '1') result = "(" + result + "+n/n)";
    }
    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int k;
    cin >> k;

    // S = 1 - n * (1 - (n-1) * (1 - ...)).
    // Its expansion is the alternating sum of falling factorials.
    string sum = "n/n";
    for (int j = k - 1; j >= 0; --j) {
        sum = "(n/n-(n-" + make_constant(j) + ")*" + sum + ")";
    }

    // This is +1 for even n and -1 for odd n.
    // round(n/2) is n/2 for even n and (n+1)/2 for odd n.
    string sign = "((n/n)+(n+n)*(n-(n+n)*round(n/(n+n))))";
    cout << "(" << sign << "*" << sum << ")\n";
}
