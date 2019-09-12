class Solution {
public:
    int numSubarrayBoundedMax(vector<int>& A, int L, int R) {
        int rv = 0;
        int l = 0;
        while (true) {
            while (l < A.size() && A[l] > R) {
                ++l;
            }
            if (l >= A.size()) {
                break;
            }

            int m = A[l];
            for (int r = l; r < A.size(); ++r) {
                if (A[r] > m) {
                    m = A[r];
                }
                if (m >= L && m <= R) {
                    ++rv;
                }
                if (m > R) {
                    break;
                }
            }

            ++l;
        }
        return rv;
    }
};
