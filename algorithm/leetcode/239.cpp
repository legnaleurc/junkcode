class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        multiset<int> cache;
        vector<int> rv;
        for (int i = 0; i < nums.size(); ++i) {
            cache.insert(nums[i]);
            if (cache.size() < k) {
                continue;
            }
            if (cache.size() > k) {
                cache.erase(cache.find(nums[i-k]));
            }
            rv.push_back(*rbegin(cache));
        }
        return rv;
    }
};
