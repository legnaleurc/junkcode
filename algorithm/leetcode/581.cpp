class Solution {
public:
    int findUnsortedSubarray(vector<int>& nums) {
        if (nums.empty()) {
            return 0;
        }
        int l = 0;
        int r = nums.size() - 1;
        while (l < nums.size() - 1 && nums[l] <= nums[l+1]) {
            ++l;
        }
        while (r > 0 && nums[r] >= nums[r-1]) {
            --r;
        }
        if (l == r) {
            return 0;
        }

        int min_ = nums[l];
        int max_ = nums[r];
        for (int i = l; i <= r; ++i) {
            min_ = min(nums[i], min_);
            max_ = max(nums[i], max_);
        }

        while (l > 0 && min_ < nums[l-1]) {
            --l;
            min_ = min(nums[l], min_);
        }
        while (r < nums.size() -1 && max_ > nums[r+1]) {
            ++r;
            max_ = max(nums[r], max_);
        }

        if (r <= l) {
            return 0;
        }
        return r - l + 1;
    }
};
