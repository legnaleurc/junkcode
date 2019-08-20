class Solution {
public:
    string convert(string s, int numRows) {
        if (s.size() <= 1) {
            return s;
        }
        if (numRows <= 1) {
            return s;
        }
        ostringstream sout;
        auto stride = (numRows - 1) * 2;
        for (int i = 1; i <= numRows; ++i) {
            for (int j = i - 1; j < s.size(); j += stride) {
                sout << s[j];
                if (i == 1 || i == numRows) {
                    continue;
                }
                int k = j + (numRows - i) * 2;
                if (k < s.size()) {
                    sout << s[k];
                }
            }
        }
        return sout.str();
    }
};
