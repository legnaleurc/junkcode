class Solution {
public:
    int minimumTotal(vector<vector<int>>& triangle) {
        if (triangle.empty()) {
            return 0;
        }
        vector<pair<int, int>> cache;
        for (int i = 0 ; i < triangle.back().size(); ++i) {
            cache.push_back({ i, triangle.back()[i] });
        }

        for (int row = triangle.size() - 2; row >= 0; --row) {
            const auto & r = triangle[row];
            for (int i = 0; i < cache.size(); ++i) {
                auto & c = cache[i];
                if (c.first <= 0) {
                    c = { c.first, c.second + r.front() };
                } else if (c.first >= r.size()) {
                    c = { c.first - 1, c.second + r.back() };
                } else {
                    if (r[c.first-1] < r[c.first]) {
                        c = { c.first - 1, c.second + r[c.first-1] };
                    } else {
                        c = { c.first, c.second + r[c.first] };
                    }
                }
            }
        }

        return (*min_element(begin(cache), end(cache), [](const pair<int, int>& a, const pair<int, int>& b) -> bool {
            return a.second < b.second;
        })).second;
    }
};
