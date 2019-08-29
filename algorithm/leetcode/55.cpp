class Solution {
public:
    bool canJump(vector<int>& nums) {
        int rv = nums.size() - 1;
        for (int i = nums.size() - 1; i >= 0; --i) {
            if (i + nums[i] >= rv) {
                rv = i;
            }
        }
        return rv == 0;
    }
};
