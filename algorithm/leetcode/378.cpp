class Solution {
    using Pair = pair<int, int>;
public:
    int kthSmallest(vector<vector<int>>& matrix, int k) {
        auto cmp = [](const Pair & a, const Pair & b) -> bool {
            return a.first > b.first;
        };
        priority_queue<Pair, vector<Pair>, decltype(cmp)> cache(cmp);
        for (int i = 0; i < matrix.size(); ++i) {
            cache.push({ matrix[i].front(), i });
        }
        k--;
        while (k--) {
            auto p = cache.top();
            auto & v = matrix[p.second];
            v.erase(begin(v));
            if (!v.empty()) {
                cache.push({ v.front(), p.second });
            }
            cache.pop();
        }
        return cache.top().first;
    }
};
