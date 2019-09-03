class Solution {
public:
    vector<int> diffWaysToCompute(string input) {
        return compute(input, 0, input.size());
    }

    vector<int> compute(const string& input, int b, int e) {
        if (b >= e) {
            return {};
        }
        vector<int> rv;
        int ob = b;
        while (nextExpression(input, ob, e, ob)) {
            int oe = ob + 1;
            auto leftList = compute(input, b, ob);
            auto rightList = compute(input, oe, e);
            for (auto l : leftList) {
                for (auto r : rightList) {
                    rv.push_back(evaluate(input[ob], l, r));
                }
            }
            ob = oe;
        }
        if (rv.empty()) {
            rv.push_back(stoi(input.substr(b, e - b)));
        }
        return rv;
    }

    bool nextExpression(const string& input, int b, int e, int& o) {
        auto i = input.find_first_of("+-*", b);
        if (i == string::npos || i >= e) {
            return false;
        }
        o = i;
        return true;
    }

    int evaluate(char o, int l, int r) {
        switch (o) {
            case '+':
                return l + r;
            case '-':
                return l - r;
            case '*':
                return l * r;
        }
        return 0;
    }
};
