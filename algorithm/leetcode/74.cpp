class Solution {
public:
    bool searchMatrix(vector<vector<int>>& matrix, int target) {
        if (matrix.empty()) {
            return false;
        }
        if (matrix.front().empty()) {
            return false;
        }
        int row = 0;
        int col = matrix.front().size() - 1;
        while (row < matrix.size() && col >= 0) {
            if (target < matrix[row][col]) {
                --col;
            } else if (target > matrix[row][col]) {
                ++row;
            } else {
                return true;
            }
        }
        return false;
    }
};
