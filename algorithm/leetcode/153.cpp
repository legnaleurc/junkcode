class Solution {
public:
    int findMin(vector<int>& nums) {
        return binarySearch(nums, 0, nums.size());
    }

    int binarySearch(const vector<int>& nums, int b, int e) {
        if (e - b == 1) {
            return nums[b];
        }
        if (nums[b] <= nums[e-1]) {
            // this section is sorted
            return nums[b];
        }
        int m = b + (e - b) / 2;
        int l = binarySearch(nums, b, m);
        int r = binarySearch(nums, m, e);
        return min(l, r);
    }
};
