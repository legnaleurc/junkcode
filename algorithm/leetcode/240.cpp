class Solution {
public:
    bool searchMatrix(vector<vector<int>>& matrix, int target) {
        if (matrix.empty()) {
            return false;
        }
        return searchMatrix(matrix, 0, matrix.size(), 0, matrix.back().size(), target);
    }
    
    bool searchMatrix(
        const vector<vector<int>>& matrix,
        int rb, int re,
        int cb, int ce,
        int target)
    {
        if (rb >= re) {
            return false;
        }
        if (cb >= ce) {
            return false;
        }
        if (re - rb == 1 && ce - cb == 1) {
            return target == matrix[rb][cb];
        }
        int rm = rb + (re - rb) / 2;
        int cm = cb + (ce - cb) / 2;
        if (matrix[rm][cm] == target) {
            return true;
        }
        if (searchMatrix(matrix, rm, re, cb, cm, target)) {
            return true;
        }
        if (searchMatrix(matrix, rb, rm, cm, ce, target)) {
            return true;
        }
        if (matrix[rm][cm] > target) {
            return searchMatrix(matrix, rb, rm, cb, cm, target);
        } else {
            return searchMatrix(matrix, rm, re, cm, ce, target);
        }
        return false;
    }
};
