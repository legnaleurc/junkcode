class Solution {
public:
    int longestConsecutive(vector<int>& nums) {
        if (nums.empty()) {
            return 0;
        }
        sort(nums.begin(), nums.end());
        int l = 1;
        int rv = 1;
        for (int i = 1; i < nums.size(); ++i) {
            // ignore same number
            if (nums[i] == nums[i-1]) {
                continue;
            }
            if (nums[i] - nums[i-1] == 1) {
                ++l;
            } else {
                // disconnected, update max
                rv = max(rv, l);
                l = 1;
            }
        }
        rv = max(rv, l);
        return rv;
    }
};
