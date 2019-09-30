class Solution {
public:
    int longestStrChain(vector<string>& words) {
        sort(begin(words), end(words), [](const string & a, const string & b) -> bool {
            return a.size() > b.size();
        });
        unordered_map<string, int> cache;
        for (const auto & word : words) {
            cache.insert({ word, 1 });
        }
        int rv = 1;
        for (const auto & word : words) {
            auto now = cache[word];
            for (int i = 0; i < word.size(); ++i) {
                auto tmp = word;
                tmp.erase(next(begin(tmp), i));
                auto it = cache.find(tmp);
                if (it == end(cache)) {
                    continue;
                }
                it->second = max(it->second, now + 1);
                rv = max(rv, it->second);
            }
        }
        return rv;
    }
};
