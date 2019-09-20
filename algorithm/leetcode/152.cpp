class Solution {
public:
    int maxProduct(vector<int>& nums) {
        if (nums.empty()) {
            return 0;
        }
        int max_ = nums[0];
        for (int b = 0; b < nums.size(); ++b) {
            int p = nums[b];
            max_ = max(p, max_);
            for (int e = b + 1; e < nums.size(); ++e) {
                p *= nums[e];
                max_ = max(p, max_);
            }
        }
        return max_;
    }
};
