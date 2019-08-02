class Solution {
public:
    vector<vector<int>> subsets(vector<int>& nums) {
        sort(begin(nums), end(nums));
        vector<vector<int>> rv;
        vector<int> tmp;
        traverse(rv, nums, 0, tmp);
        return rv;
    }
    
    void traverse(vector<vector<int>>& rv, const vector<int>& nums, int i, vector<int>& tmp) {
        if (i == nums.size()) {
            rv.push_back(tmp);
            return;
        }
        traverse(rv, nums, i + 1, tmp);
        tmp.push_back(nums[i]);
        traverse(rv, nums, i + 1, tmp);
        tmp.pop_back();
    }
};
