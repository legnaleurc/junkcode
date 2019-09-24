class NumMatrix {
    vector<vector<int>> matrix_;
public:
    NumMatrix(vector<vector<int>>& matrix)
        : matrix_(move(matrix))
    {
        if (matrix_.empty() || matrix_.front().empty()) {
            return;
        }
        for (int row = 1; row < matrix_.size(); ++row) {
            matrix_[row].front() += matrix_[row-1].front();
        }
        for (int col = 1; col < matrix_.front().size(); ++col) {
            matrix_.front()[col] += matrix_.front()[col-1];
        }
        for (int row = 1; row < matrix_.size(); ++row) {
            for (int col = 1; col < matrix_[row].size(); ++col) {
                matrix_[row][col] = matrix_[row][col] + matrix_[row][col-1] + matrix_[row-1][col] - matrix_[row-1][col-1];
            }
        }
    }
    
    int sumRegion(int row1, int col1, int row2, int col2) const {
        if (row2 < 0 || col2 < 0) {
            return 0;
        }
        int rv = matrix_[row2][col2];
        if (row1 > 0) {
            rv -= matrix_[row1-1][col2];
        }
        if (col1 > 0) {
            rv -= matrix_[row2][col1-1];
        }
        if (row1 > 0 && col1 > 0) {
            rv += matrix_[row1-1][col1-1];
        }
        return rv;
    }
};

/**
 * Your NumMatrix object will be instantiated and called as such:
 * NumMatrix* obj = new NumMatrix(matrix);
 * int param_1 = obj->sumRegion(row1,col1,row2,col2);
 */
