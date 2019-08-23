class Solution {
public:
    int reverse(int x) {
        bool negative = false;
        int64_t rv = 0;
        int64_t y = x;
        if (y < 0) {
            negative = true;
            y *= -1;
        }
        while (y > 0) {
            int digit = y % 10;
            y /= 10;
            rv *= 10;
            rv += digit;
        }
        if (negative) {
            rv *= -1;
        }
        if (rv > INT_MAX || rv < INT_MIN) {
            return 0;
        }
        return rv;
    }
};
