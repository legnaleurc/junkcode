class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {
        if (nums.size() < 3) {
            return {};
        }
        sort(nums.begin(), nums.end());

        vector<vector<int>> rv;
        for (int i = 0; i < nums.size() - 2; ++i) {
            if (i > 0 && nums[i] == nums[i-1]) {
                continue;
            }

            int left = i + 1;
            int right = nums.size() - 1;
            while (left < right) {
                int two = nums[left] + nums[right];
                int one = 0 - nums[i];
                if (two == one) {
                    rv.push_back({nums[i], nums[left], nums[right]});

                    ++left;
                    while (nums[left] == nums[left-1]) {
                        ++left;
                    }

                    --right;
                    while (nums[right] == nums[right+1]) {
                        --right;
                    }
                } else if (two < one) {
                    ++left;
                } else {
                    --right;
                }
            }
        }
        return rv;
    }
};
