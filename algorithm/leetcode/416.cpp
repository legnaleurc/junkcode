class Solution {
public:
    bool canPartition(vector<int>& nums) {
        int sum = accumulate(begin(nums), end(nums), 0);
        if (sum % 2 != 0) {
            return false;
        }
        sum /= 2;
        vector<bool> cache(sum + 1, false);
        cache[0] = true;
        for (int num : nums) {
            for (int sub = sum; sub >= num; --sub) {
                if (cache[sub - num]) {
                    cache[sub] = true;
                }
            }
            if (cache[sum]) {
                break;
            }
        }
        return cache[sum];
    }
};
