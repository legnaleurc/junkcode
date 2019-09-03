class Solution {
public:
    vector<int> searchRange(vector<int>& nums, int target) {
        auto b = lowerBound(nums, 0, nums.size(), target);
        auto e = upperBound(nums, 0, nums.size(), target);
        if (b == e) {
            return { -1, -1 };
        }
        return { b, e - 1 };
    }

    int lowerBound(const vector<int>& nums, int b, int e, int target) const {
        int d = e - b;
        int step = 0;
        while (d > 0) {
            int i = b;
            step = d / 2;
            i += step;
            if (nums[i] < target) {
                b = ++i;
                d -= step + 1;
            } else {
                d = step;
            }
        }
        return b;
    }

    int upperBound(const vector<int>& nums, int b, int e, int target) const {
        int d = e - b;
        int step = 0;
        while (d > 0) {
            int i = b;
            step = d / 2;
            i += step;
            if (nums[i] <= target) {
                b = ++i;
                d -= step + 1;
            } else {
                d = step;
            }
        }
        return b;
    }
};
