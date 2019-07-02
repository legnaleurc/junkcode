class Solution {
public:
    string longestCommonPrefix(vector<string>& strs) {
        if (strs.empty()) {
            return "";
        }
        string rv = strs[0];
        for (int i = 1; i < strs.size(); ++i) {
            rv = commonPrefix(rv, strs[i]);
        }
        return rv;
    }
    
    string commonPrefix(const string & a, const string & b) {
        int i = 0;
        while (i < a.size() && i < b.size() && a[i] == b[i]) {
            ++i;
        }
        return a.substr(0, i);
    }
};
