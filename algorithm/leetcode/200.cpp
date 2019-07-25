class Solution {
public:
    int numIslands(vector<vector<char>>& grid) {
        int sum = 0;
        for (int row = 0; row < grid.size(); ++row) {
            auto & flow = grid[row];
            for (int col = 0; col < flow.size(); ++col) {
                if (flow[col] != '1') {
                    continue;
                }
                mark(grid, row, col);
                ++sum;
            }
        }
        return sum;
    }
    
    void mark(vector<vector<char>>& grid, int row, int col) {
        if (grid[row][col] != '1') {
            return;
        }
        grid[row][col] = '2';
        if (row - 1 >= 0) {
            mark(grid, row - 1, col);
        }
        if (col - 1 >= 0) {
            mark(grid, row, col - 1);
        }
        if (row + 1 < grid.size()) {
            mark(grid, row + 1, col);
        }
        if (col + 1 < grid[row].size()) {
            mark(grid, row, col + 1);
        }
    }
};
