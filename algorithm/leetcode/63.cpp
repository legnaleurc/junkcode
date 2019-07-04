class Solution {
public:
    int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
        vector<vector<int64_t>> cache;
        cache.reserve(obstacleGrid.size());
        for (int row = 0; row < obstacleGrid.size(); ++row) {
            cache.push_back(vector<int64_t>(obstacleGrid[row].size(), 0LL));
        }
        for (int row = cache.size() - 1; row >= 0; --row) {
            auto & line = cache[row];
            for (int col = line.size() - 1; col >= 0; --col) {
                if (obstacleGrid[row][col]) {
                    line[col] = 0;
                    continue;
                }
                if (row == cache.size() - 1 && col == line.size() - 1) {
                    line[col] = 1;
                } else if (row == cache.size() - 1) {
                    line[col] = cache[row][col+1];
                } else if (col == line.size() - 1) {
                    line[col] = cache[row+1][col];
                } else {
                    line[col] = cache[row+1][col] + cache[row][col+1];
                }
            }
        }
        return cache[0][0];
    }
};
