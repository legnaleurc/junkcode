class Solution {
public:
    string decodeString(string s) {
        int i = 0;
        return parseString(s, i, 1);
    }
    
    string parseString(const string& s, int& i, int n) {
        string rv;
        string repeat;
        for (; i < s.size(); ++i) {
            char c = s[i];
            if (c == ']') {
                ostringstream sout;
                for (int i = 0; i < n; ++i) {
                    sout << rv;
                }
                rv = sout.str();
                break;
            }
            if (isdigit(c)) {
                repeat.push_back(c);
            } else if (c == '[') {
                ++i;
                int n = stoi(repeat);
                repeat = "";
                auto ss = parseString(s, i, n);
                rv += ss;
            } else {
                rv += c;
            }
        }
        return rv;
    }
};
