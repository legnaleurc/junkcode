class Solution {
public:
    int minimumTotal(vector<vector<int>>& triangle) {
        if (triangle.empty()) {
            return 0;
        }
        vector<int> cache = triangle.back();

        for (int row = triangle.size() - 2; row >= 0; --row) {
            const auto & r = triangle[row];
            for (int i = 0; i < row + 1; ++i) {
                cache[i] = min(cache[i], cache[i+1]) + r[i];
            }
        }

        return cache.front();
    }
};
