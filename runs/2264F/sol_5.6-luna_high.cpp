#include <bits/stdc++.h>

using namespace std;

static vector<string> constant_expressions(int limit) {
    constexpr int max_degree = 8;
    struct Expr {
        string text;
        char top = 0;
    };
    auto combine = [](const Expr &left, const Expr &right, char op) {
        auto body = [&](const Expr &e) {
            if (e.top == op) return e.text.substr(1, e.text.size() - 2);
            return e.text;
        };
        return Expr{"(" + body(left) + op + body(right) + ")", op};
    };
    vector<vector<Expr>> h(max_degree + 1, vector<Expr>(limit + 1));
    for (int degree = 1; degree <= max_degree; ++degree) {
        h[degree][1] = (degree == 1 ? Expr{"n", 0} : combine(h[degree - 1][1], Expr{"n", 0}, '*'));
        for (int value = 2; value <= limit; ++value) {
            Expr best;
            auto consider = [&](Expr candidate) {
                if (best.text.empty() || candidate.text.size() < best.text.size()) best = move(candidate);
            };
            for (int a = 1; a * 2 <= value; ++a)
                consider(combine(h[degree][a], h[degree][value - a], '+'));
            for (int left_degree = 1; left_degree < degree; ++left_degree) {
                int right_degree = degree - left_degree;
                for (int a = 1; a * a <= value; ++a) {
                    if (value % a == 0) {
                        int b = value / a;
                        consider(combine(h[left_degree][a], h[right_degree][b], '*'));
                        if (a != b)
                            consider(combine(h[left_degree][b], h[right_degree][a], '*'));
                    }
                }
            }
            if (degree > 1) consider(combine(h[degree - 1][value], Expr{"n", 0}, '*'));
            h[degree][value] = move(best);
        }
    }
    vector<string> result(limit + 1);
    result[0] = "n-n";
    result[1] = "n/n";
    for (int value = 2; value <= limit; ++value) {
        size_t best_size = numeric_limits<size_t>::max();
        for (int degree = 1; degree <= max_degree; ++degree) {
            string denominator = "n";
            for (int j = 1; j < degree; ++j) denominator += "*n";
            string candidate = h[degree][value].text + "/(" + denominator + ")";
            if (candidate.size() < best_size) {
                best_size = candidate.size();
                result[value] = move(candidate);
            }
        }
    }
    return result;
}

static vector<int> convert_decimal(string value, int base) {
    vector<int> result;
    while (value != "0") {
        string quotient;
        int remainder = 0;
        for (char ch : value) {
            int current = remainder * 10 + (ch - '0');
            int digit = current / base;
            remainder = current % base;
            if (!quotient.empty() || digit != 0) quotient.push_back(char('0' + digit));
        }
        result.push_back(remainder);
        value = quotient.empty() ? "0" : quotient;
    }
    reverse(result.begin(), result.end());
    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int k;
    cin >> k;
    if (k == 2) {
        cout << "n-n/n\n";
        return 0;
    }
    if (k == 3) {
        cout << "n+round(n-n*(n/(n+n)))-n\n";
        return 0;
    }

    // A continued-fraction approximation of 1/e, accurate enough through 50!.
    const string numerator_decimal =
        "16628883765954330083456633789321";
    const int base = 10;
    const string one = "n/n";
    const vector<string> constants = constant_expressions(100);

    auto encode = [&](const string &digits, int selected_base) {
        vector<int> ds = convert_decimal(digits, selected_base);
        string result = constants[ds[0]];
        for (size_t i = 1; i < ds.size(); ++i) {
            if (ds[i] == 0)
                result = "(" + result + "*(" + constants[selected_base] + "))";
            else
                result = "(" + result + "*(" + constants[selected_base] + ")+" + constants[ds[i]] + ")";
        }
        return result;
    };

    const string denominator_decimal = "45201992568551270405895259991443";

    string factorial = constants[2];
    for (int i = 3; i <= 50; ++i) {
        string cim1 = constants[i - 1];
        string numerator = "(n-" + cim1 + ")";
        string denominator = "(n+n-" + constants[2 * i - 1] + ")";
        string indicator = "round(" + numerator + "/" + denominator + ")";
        string factor = "(" + one + "+(" + cim1 + ")*" + indicator + ")";
        factorial = "(" + factorial + "*" + factor + ")";
    }

    string answer = "round((" + factorial + "*" + encode(numerator_decimal, base) + ")/" +
                    encode(denominator_decimal, base) + ")";
    cout << answer << '\n';
}
