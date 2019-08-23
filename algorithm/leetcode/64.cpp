class Solution {
public:
    int minPathSum(vector<vector<int>>& grid) {
        if (grid.empty()) {
            return 0;
        }

        // top
        for (int i = 1; i < grid[0].size(); ++i) {
            grid[0][i] += grid[0][i-1];
        }

        // left
        for (int i = 1; i < grid.size(); ++i) {
            grid[i][0] += grid[i-1][0];
        }

        // rest
        for (int row = 1; row < grid.size(); ++row) {
            for (int col = 1; col < grid[row].size(); ++col) {
                grid[row][col] += min(grid[row-1][col], grid[row][col-1]);
            }
        }

        return grid.back().back();
    }
};
