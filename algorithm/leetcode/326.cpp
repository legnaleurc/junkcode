class Solution {
public:
    bool isPowerOfThree(int n) {
        while (true) {
            if (n == 0) {
                return false;
            }
            if (n == 1 || n == 3) {
                return true;
            }
            if (n % 3 != 0) {
                return false;
            }
            n = n / 3;
        }
        return false;
    }
};
