class Solution {
public:
    int hammingWeight(uint32_t n) {
        int rv = 0;
        while (n) {
            if (n & 1) {
                ++rv;
            }
            n = n >> 1;
        }
        return rv;
    }
};
