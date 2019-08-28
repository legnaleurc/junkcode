class Solution {
public:
    string longestPalindrome(string s) {
        string rv;
        for (int i = 0; i < s.size(); ++i) {
            if (s.size() - i < rv.size()) {
                continue;
            }
            for (int j = i + 1; j <= s.size(); ++j) {
                if (j - i < rv.size()) {
                    continue;
                }
                if (!isPalindrome(s, i, j)) {
                    continue;
                }
                rv = s.substr(i, j - i);
            }
        }
        return rv;
    }
    
    bool isPalindrome(const string& s, int i, int j) {
        while (i < j) {
            if (s[i] != s[j-1]) {
                return false;
            }
            ++i;
            --j;
        }
        return true;
    }
};
