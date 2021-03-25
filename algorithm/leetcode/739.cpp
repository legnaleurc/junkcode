class Solution {
public:
    vector<int> dailyTemperatures(vector<int>& T) {
        if (T.empty()) {
            return {};
        }
        vector<int> rv;
        unordered_map<int, vector<int>> cache;
        for (int i = 0; i < T.size(); ++i) {
            rv.push_back(0);

            auto it = cache.find(T[i]);
            if (it == end(cache)) {
                cache.insert({ T[i], { i } });
            } else {
                it->second.push_back(i);
            }

            for (it = begin(cache); it != end(cache);) {
                if (it->first < T[i]) {
                    for (auto j : it->second) {
                        rv[j] = i - j;
                    }
                    it = cache.erase(it);
                } else {
                    ++it;
                }
            }
        }
        return rv;
    }
};
