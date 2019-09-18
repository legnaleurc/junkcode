class Solution {
public:
    bool isAnagram(string s, string t) {
        unordered_multiset a(begin(s), end(s));
        unordered_multiset b(begin(t), end(t));
        return a == b;
    }
};
