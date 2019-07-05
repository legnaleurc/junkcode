class Solution {
public:
    bool containsNearbyDuplicate(vector<int>& nums, int k) {
        unordered_map<int, int> cache;
        for (int i = 0; i < nums.size(); ++i) {
            int v = nums[i];
            auto it = cache.find(v);
            if (it == end(cache)) {
                cache.insert(make_pair(v, i));
                continue;
            }
            if (i - it->second <= k) {
                return true;
            }
            it->second = i;
        }
        return false;
    }
};
