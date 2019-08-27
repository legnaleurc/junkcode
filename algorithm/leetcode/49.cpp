class Solution {
public:
    vector<vector<string>> groupAnagrams(vector<string>& strs) {
        unordered_map<string, vector<string>> cache;
        for (const auto & s : strs) {
            auto k = s;
            sort(begin(k), end(k));
            auto it = cache.find(k);
            if (it != end(cache)) {
                it->second.push_back(s);
            } else {
                cache.insert({ k, { s } });
            }
        }
        vector<vector<string>> rv;
        for (const auto & p : cache) {
            rv.push_back(p.second);
        }
        return rv;
    }
};
