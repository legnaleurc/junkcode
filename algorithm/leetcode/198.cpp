class Solution {
public:
    int rob(vector<int>& nums) {
        if (nums.empty()) {
            return 0;
        }
        int a = 0;
        int b = nums[0];
        for (int i = 1; i < nums.size(); ++i) {
            int c = max(a + nums[i], b);
            a = b;
            b = c;
        }
        return max(a, b);
    }
};
