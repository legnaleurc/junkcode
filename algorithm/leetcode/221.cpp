class Solution {
public:
    int maximalSquare(vector<vector<char>>& matrix) {
        for (auto & row : matrix) {
            for (auto & col : row) {
                if (col == '0') {
                    col = 0;
                } else {
                    col = 1;
                }
            }
        }

        for (int row = 1; row < matrix.size(); ++row) {
            for (int col = 1; col < matrix[row].size(); ++col) {
                if (matrix[row][col] == 0) {
                    continue;
                }
                matrix[row][col] = min({ matrix[row-1][col], matrix[row][col-1], matrix[row-1][col-1] }) + 1;
            }
        }

        int rv = 0;
        for (auto & row : matrix) {
            for (auto & col : row) {
                if (col > rv) {
                    rv = col;
                }
            }
        }

        return rv * rv;
    }
};
