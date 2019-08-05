class Solution {
public:
    void rotate(vector<vector<int>>& matrix) {
        reverse(begin(matrix), end(matrix));
        for (int row = 0; row < matrix.size(); ++row) {
            for (int col = 0; col < row; ++col) {
                swap(matrix[row][col], matrix[col][row]);
            }
        }
    }
};
