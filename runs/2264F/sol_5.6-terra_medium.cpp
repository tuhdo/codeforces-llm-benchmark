#include <bits/stdc++.h>
using namespace std;

// The generated expression uses
//   E(n) = 1 - n(1 - (n-1)(1 - ...)).
// At n, the factor n-n is zero, so the tail disappears and
// E(n) = (-1)^n * !n.  A short expression for (-1)^n fixes the sign.
string constantValue(int value, const string& one) {
    if (value == 0) return "(" + one + "-" + one + ")";
    if (value == 1) return one;

    string result = one;
    string bits = bitset<6>(value).to_string();
    size_t first = bits.find('1');
    for (size_t i = first + 1; i < bits.size(); ++i) {
        result = "(" + result + "+" + result + ")";
        if (bits[i] == '1') result = "(" + result + "+" + one + ")";
    }
    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int k;
    cin >> k;

    const string one = "n/n";
    const string two = "(" + one + "+" + one + ")";

    string nested = one;
    for (int offset = k - 1; offset >= 0; --offset) {
        string factor = offset == 0 ? "n" : "(n-" + constantValue(offset, one) + ")";
        nested = "(" + one + "-" + factor + "*" + nested + ")";
    }

    // round(n/2) is ceil(n/2), since ties round upward.  Thus this is
    // 1 for even n and -1 for odd n.
    string parity = "(" + one + "+" + two + "*(n-" + two + "*round(n/" + two + ")) )";
    // Remove the harmless readability space: output must contain only DC characters.
    parity.erase(remove(parity.begin(), parity.end(), ' '), parity.end());
    cout << "(" << parity << "*" << nested << ")\n";
}
