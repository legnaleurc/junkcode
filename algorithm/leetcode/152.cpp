class Solution {
public:
    int maxProduct(vector<int>& nums) {
        if (nums.empty()) {
            return 0;
        }
        // need to keep track minimum values because -1 * -1 = 1
        int min_ = nums[0];
        int max_ = nums[0];
        int rv = nums[0];
        for (int i = 1; i < nums.size(); ++i) {
            int a = min({ nums[i], min_ * nums[i], max_ * nums[i] });
            int b = max({ nums[i], min_ * nums[i], max_ * nums[i] });
            rv = max(rv, b);
            min_ = a;
            max_ = b;
        }
        return rv;
    }
};
