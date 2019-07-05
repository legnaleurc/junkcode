class Solution {
public:
    bool containsDuplicate(vector<int>& nums) {
        if (nums.size() < 2) {
            return false;
        }
        sort(begin(nums), end(nums));
        for (int i = 1; i < nums.size(); ++i) {
            if (nums[i-1] == nums[i]) {
                return true;
            }
        }
        return false;
    }
};
