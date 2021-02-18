class Solution {
public:
    int firstMissingPositive(vector<int>& nums) {
        sort(nums.begin(), nums.end());
        int rv = 1;
        for (int i = 0; i < nums.size(); ++i) {
            if (nums[i] <= 0) {
                continue;
            }
            if (nums[i] > rv) {
                break;
            }
            if (nums[i] == rv) {
                ++rv;
            }
        }
        return rv;
    }
};
