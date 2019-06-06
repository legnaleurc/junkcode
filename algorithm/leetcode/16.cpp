class Solution {
public:
    int threeSumClosest(vector<int>& nums, int target) {
        sort(nums.begin(), nums.end());

        int rv = nums[0] + nums[1] + nums[2];

        for (int i = 0; i < nums.size() - 2; ++i) {
            int goal = target - nums[i];
            int left = i + 1;
            int right = nums.size() - 1;
            while (left < right) {
                int sum = nums[left] + nums[right];
                if (abs(sum - goal) < abs(rv - target)) {
                    rv = nums[i] + sum;
                }

                if (sum < goal) {
                    ++left;
                } else if (sum > goal) {
                    --right;
                } else {
                    return target;
                }
            }
        }

        return rv;
    }
};
