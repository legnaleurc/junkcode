class Solution {
public:
    vector<int> countBits(int num) {
        vector<int> rv{{ 0 }};
        for (int i = 1; i <= num; ++i) {
            rv.push_back(rv[i >> 1] + (i & 0x1));
        }
        return rv;
    }
};
