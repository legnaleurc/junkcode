class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        int rv = 0;
        unordered_map<char, string::iterator> table;
        auto l = begin(s), r = begin(s), e = end(s);
        for (; r != e; ++r) {
            rv = max(rv, static_cast<int>(distance(l, r)));
            auto it = table.find(*r);
            if (it == end(table)) {
                table.insert({*r, next(r)});
            } else {
                l = it->second;
                it->second = next(r);
            }
        }
        rv = max(rv, static_cast<int>(distance(l, r)));
        return rv;
    }
};
