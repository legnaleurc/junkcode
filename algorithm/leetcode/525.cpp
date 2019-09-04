class Solution {
public:
    int findMaxLength(vector<int>& nums) {
        int rv = 0;
        int sum = 0;
        unordered_map<int, int> cache;
        cache.insert({ sum, -1 });
        for (int i = 0; i < nums.size(); ++i) {
            if (nums[i] == 1) {
                sum += 1;
            } else {
                sum -= 1;
            }
            auto it = cache.find(sum);
            if (it == end(cache)) {
                cache.insert({ sum, i });
            } else {
                rv = max(rv, i - it->second);
            }
        }
        return rv;
    }
};
