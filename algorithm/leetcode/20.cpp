class Solution {
public:
    bool isValid(string s) {
        stack<char> table;
        for (auto c : s) {
            switch (c) {
                case '(':
                case '{':
                case '[':
                    table.push(c);
                    break;
                case ')':
                    if (table.empty() || table.top() != '(') {
                        return false;
                    }
                    table.pop();
                    break;
                case '}':
                    if (table.empty() || table.top() != '{') {
                        return false;
                    }
                    table.pop();
                    break;
                case ']':
                    if (table.empty() || table.top() != '[') {
                        return false;
                    }
                    table.pop();
                    break;
                default:
                    return false;
            }
        }
        return table.empty();
    }
};
