class Solution {
public:
    vector<int> findDisappearedNumbers(vector<int>& nums) {
        vector<int> rv(nums.size(), 0);
        iota(begin(rv), end(rv), 1);
        for (auto i : nums) {
            rv[i-1] = 0;
        }
        auto it = remove(begin(rv), end(rv), 0);
        rv.erase(it, end(rv));
        return rv;
    }
};
