class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        if (nums.empty()) {
            return 0;
        }
        int i = 1;
        int e = nums.size();
        bool dup = false;
        for (; i < e; ++i) {
            if (nums[i] == nums[i-1]) {
                if (!dup) {
                    dup = true;
                } else {
                    reverse(&nums[i], &nums[e]);
                    while (e > i && nums[i-1] == nums[e-1]) {
                        e -= 1;
                    }
                    if (i < e) {
                        reverse(&nums[i], &nums[e]);
                    } else {
                        break;
                    }
                    dup = false;
                }
            } else {
                dup = false;
            }
        }
        return i;
    }
};
