class Solution {
public:
    string reverseWords(string s) {
        istringstream sin(s);
        string token;
        stack<string> stack_;
        while (sin >> token) {
            stack_.push(token);
        }
        if (stack_.empty()) {
            return "";
        }
        ostringstream sout;
        sout << stack_.top();
        stack_.pop();
        while (!stack_.empty()) {
            sout << " " << stack_.top();
            stack_.pop();
        }
        return sout.str();
    }
};
