// O(n^2)
class Solution {
public:
    string longestPalindrome(string s) {
        auto rv = make_pair(0, 0);
        for (int i = 0; i < s.size(); ++i) {
            auto odd = expand(s, i, i);
            auto even = expand(s, i, i + 1);
            if (odd.second - odd.first > rv.second - rv.first) {
                rv = odd;
            }
            if (even.second - even.first > rv.second - rv.first) {
                rv = even;
            }
        }
        return s.substr(rv.first, rv.second - rv.first);
    }
    
    pair<int, int> expand(const string& s, int b, int e) {
        while (b >= 0 && e < s.size() && s[b] == s[e]) {
            --b;
            ++e;
        }
        return {b + 1, e};
    }
};


// O(n^3)
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
