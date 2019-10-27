class Solution {
public:
    int maxProduct(vector<int>& nums) {
        if (nums.empty()) {
            return 0;
        }
        int max_ = nums[0];
        for (int b = 0; b < nums.size(); ++b) {
            int p = 1;
            for (int e = b; e < nums.size(); ++e) {
                p *= nums[e];
                max_ = max(p, max_);
            }
        }
        return max_;
    }
};