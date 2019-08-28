class Solution {
public:
    string longestPalindrome(string s) {
        int l = 0;
        int r = 0;
        for (int i = 0; i < s.size(); ++i) {
            if (s.size() - i <= r - l) {
                continue;
            }
            for (int j = i + 1; j <= s.size(); ++j) {
                if (j - i <= r - l) {
                    continue;
                }
                if (!isPalindrome(s, i, j)) {
                    continue;
                }
                l = i;
                r = j;
            }
        }
        return s.substr(l, r - l);
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
