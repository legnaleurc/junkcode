class Solution {
public:
    int rob(vector<int>& nums) {
        if (nums.empty()) {
            return 0;
        }

        int a = 0;
        int b = nums[0];
        for (int i = 1; i < nums.size() - 1; ++i) {
            int c = max(a + nums[i], b);
            a = b;
            b = c;
        }
        int rv = max(a, b);
        if (nums.size() <= 1) {
            return rv;
        }

        a = 0;
        b = nums[1];
        for (int i = 2; i < nums.size(); ++i) {
            int c = max(a + nums[i], b);
            a = b;
            b = c;
        }
        rv = max({ rv, a, b });
        return rv;
    }
};
