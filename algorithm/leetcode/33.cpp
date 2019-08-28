class Solution {
public:
    int search(vector<int>& nums, int target) {
        return binarySearch(nums, 0, nums.size(), target);
    }

    int binarySearch(const vector<int> & nums, int b, int e, int target) {
        if (b >= e) {
            return -1;
        }
        if (e - b == 1) {
            if (nums[b] == target) {
                return b;
            } else {
                return -1;
            }
        }
        int m = b + ((e - b) / 2);
        if (contains(nums, b, m, target)) {
            return binarySearch(nums, b, m, target);
        } else {
            return binarySearch(nums, m, e, target);
        }
        return -1;
    }

    bool contains(const vector<int> & nums, int b, int e, int target) {
        if (nums[b] <= nums[e-1]) {
            return target >= nums[b] && target <= nums[e-1];
        }
        if (nums[b] > target && nums[e-1] < target) {
            return false;
        }
        return true;
    }
};
