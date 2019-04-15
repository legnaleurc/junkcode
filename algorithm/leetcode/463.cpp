class Solution {
public:
    int islandPerimeter(vector<vector<int>>& grid) {
        int sum = 0;
        for (int row = 0; row < grid.size(); ++row) {
            auto & flow = grid[row];
            for (int col = 0; col < flow.size(); ++col) {
                if (flow[col] == 1) {
                    sum += 4;
                    if (col > 0 && flow[col - 1] == 1) {
                        sum -= 2;
                    }
                    if (row > 0 && grid[row - 1][col] == 1) {
                        sum -= 2;
                    }
                }
            }
        }
        return sum;
    }
};
