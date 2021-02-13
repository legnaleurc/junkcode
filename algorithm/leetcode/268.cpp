class Solution {
public:
    int missingNumber(vector<int>& nums) {
        int target = nums.size() * (nums.size() + 1) / 2;
        int sum = nums[0];
        for (int i = 1; i < nums.size(); ++i) {
            sum += nums[i];
        }
        return target - sum;
    }
};
