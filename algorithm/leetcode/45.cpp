class Solution {
public:
    int jump(vector<int>& nums) {
        if (nums.size() == 1) {
            return 0;
        }
        int rv = 1;
        int far = nums[0];
        int index = far;
        for (int i = 1; i < nums.size() - 1; ++i) {
            far = max(far, i + nums[i]);
            if (index == i) {
                index = far;
                ++rv;
            }
        }
        return rv;
    }
};
