class Solution {
public:
    vector<int> spiralOrder(vector<vector<int>>& matrix) {
        vector<int> rv;

        int topBorder = 0;
        int bottomBorder = matrix.size() - 1;
        int leftBorder = 0;
        int rightBorder = matrix[0].size() - 1;
        while (topBorder <= bottomBorder && leftBorder <= rightBorder) {
            int row = topBorder;
            int col = leftBorder;

            if (topBorder == bottomBorder) {
                for (; col <= rightBorder; ++col) {
                    rv.push_back(matrix[row][col]);
                }
            } else if (leftBorder == rightBorder) {
                for (; row <= bottomBorder; ++row) {
                    rv.push_back(matrix[row][col]);
                }
            } else {
                for (; col < rightBorder; ++col) {
                    rv.push_back(matrix[row][col]);
                }
                for (; row < bottomBorder; ++row) {
                    rv.push_back(matrix[row][col]);
                }
                for (; col > leftBorder; --col) {
                    rv.push_back(matrix[row][col]);
                }
                for (; row > topBorder; --row) {
                    rv.push_back(matrix[row][col]);
                }
            }

            ++topBorder;
            --bottomBorder;
            ++leftBorder;
            --rightBorder;
        }

        return rv;
    }
};
