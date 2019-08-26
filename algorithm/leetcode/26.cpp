class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        if (nums.empty()) {
            return 0;
        }
        int b = 0;
        int e = 0;
        while (e < nums.size()) {
            while (e < nums.size() && nums[b] == nums[e]) {
                ++e;
            }
            if (e >= nums.size()) {
                return b + 1;
            }
            b += 1;
            nums[b] = nums[e];
        }
        return 0;
    }
};
