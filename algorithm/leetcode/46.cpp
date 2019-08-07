class Solution {
public:
    vector<vector<int>> permute(vector<int>& nums) {
        vector<vector<int>> rv;
        nextPermute(rv, nums, 0);
        return rv;
    }

    void nextPermute(vector<vector<int>>& rv, vector<int>& nums, int n) {
        if (n >= nums.size()) {
            rv.push_back(nums);
            return;
        }
        for (int i = n; i < nums.size(); ++i) {
            if (i != n) {
                auto it = next(begin(nums), n + 1);
                reverse(it, end(nums));
                swap(nums[n], nums[i]);
            }
            nextPermute(rv, nums, n + 1);
        }
    }
};
