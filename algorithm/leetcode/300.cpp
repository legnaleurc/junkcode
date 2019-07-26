class Solution {
public:
    int lengthOfLIS(vector<int>& nums) {
        vector<int> dp;
        for (int i = 0; i < nums.size(); ++i) {
            auto it = lower_bound(begin(dp), end(dp), nums[i]);
            if (it == end(dp)) {
                dp.push_back(nums[i]);
            } else {
                if (nums[i] < *it) {
                    *it = nums[i];
                }
            }
        }
        return dp.size();
    }
};
