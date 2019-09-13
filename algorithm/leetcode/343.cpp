class Solution {
public:
    int integerBreak(int n) {
        if (n == 1) {
            return 0;
        }
        if (n == 2) {
            return 1;
        }
        if (n == 3) {
            return 2;
        }
        int k = n / 3;
        int p = n % 3;
        if (p == 1) {
            k -= 1;
            p = 2;
        } else if (p == 2) {
            p = 1;
        }
        return pow(3, k) * pow(2, p);
    }
};
