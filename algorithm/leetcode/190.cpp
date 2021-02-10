class Solution {
public:
    uint32_t reverseBits(uint32_t n) {
        const uint32_t mask = 1;
        uint32_t m = 0;
        m = m | (n & mask);
        for (int i = 1; i < 32; ++i) {
            m = m << 1;
            n = n >> 1;
            m = m | (n & mask);
        }
        return m;
    }
};
