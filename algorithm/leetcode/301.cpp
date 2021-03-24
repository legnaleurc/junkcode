class Solution {
public:
    vector<string> removeInvalidParentheses(string s) {
        if (s.empty()) {
            return { "" };
        }
        vector<string> rv;
        int left = 0;
        int right = 0;
        for (auto c : s) {
            if (c == '(') {
                ++left;
            } else if (c == ')') {
                if (left > 0) {
                    --left;
                } else {
                    ++right;
                }
            }
        }
        traverse(rv, s, left, right, 0);
        return rv;
    }

    void traverse(vector<string>& rv, const string& s, int left, int right, int b) {
        if (left == 0 && right == 0) {
            if (isValid(s)) {
                rv.push_back(s);
            }
            return;
        }
        for (int i = b; i < s.size(); ++i) {
            if (i > b && s[i] == s[i-1]) {
                continue;
            }
            if (s[i] == '(' && left > 0) {
                auto ss = s;
                ss.erase(i, 1);
                traverse(rv, ss, left - 1, right, i);
            }
            if (s[i] == ')' && right > 0) {
                auto ss = s;
                ss.erase(i, 1);
                traverse(rv, ss, left, right - 1, i);
            }
        }
    }

    bool isValid(const string& s) {
        int balance = 0;
        for (auto c : s) {
            if (c == '(') {
                ++balance;
            } else if (c == ')') {
                --balance;
            }
            if (balance < 0) {
                return false;
            }
        }
        return balance == 0;
    }
};
