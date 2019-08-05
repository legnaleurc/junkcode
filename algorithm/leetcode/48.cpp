class Solution {
public:
    void rotate(vector<vector<int>>& matrix) {
        for (auto & row : matrix) {
            reverse(begin(row), end(row));
        }
        for (int row = 0; row < matrix.size(); ++row) {
            const auto size = matrix.size() - row;
            for (int col = 0; col < size; ++col) {
                const auto offset = matrix.size() - 1;
                swap(matrix[row][col], matrix[offset - col][offset - row]);
            }
        }
    }
};
