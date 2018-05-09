class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        map<int, int> table;
        for (int i = 0; i < nums.size(); ++i) {
            auto it = table.find(nums[i]);
            if (it != table.end()) {
                return {it->second, i};
            }
            table.insert({target - nums[i], i});
        }
        return {-1, -1};
    }
};
