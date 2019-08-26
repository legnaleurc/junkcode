class Solution {
public:
    bool wordBreak(string s, vector<string>& wordDict) {
        unordered_set<int> cache;
        return searchWord(s, 0, s.size(), wordDict, cache);
    }

    bool searchWord(const string& s, int b, int e, const vector<string>& wordDict, unordered_set<int>& cache) {
        if (b >= e) {
            return true;
        }
        if (cache.count(b) >= 1) {
            return false;
        }
        for (const auto & word : wordDict) {
            if (!matchString(s, b, e, word)) {
                continue;
            }
            if (searchWord(s, b + word.size(), e, wordDict, cache)) {
                return true;
            }
        }
        cache.insert(b);
        return false;
    }

    bool matchString(const string& s, int b, int e, const string& word) {
        if (e - b < word.size()) {
            return false;
        }
        int i = 0;
        while (i < word.size()) {
            if (s[b] != word[i]) {
                return false;
            }
            ++b;
            ++i;
        }
        return i == word.size();
    }
};
