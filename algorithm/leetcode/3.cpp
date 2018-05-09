class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        if (s.empty()) {
            return 0;
        }

        int rv = 0;
        unordered_map<char, int> table;
        int start = -1;

        for (int i = 0; i < s.length(); ++i) {
            if (table.find(s[i]) != table.end()) {
                start = max(start, table[s[i]]);
            }
            rv = max(rv, i - start);
            table[s[i]] = i;
        }
        return rv;
    }
};
