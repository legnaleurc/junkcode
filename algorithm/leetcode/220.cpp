class Solution {
public:
    bool containsNearbyAlmostDuplicate(vector<int>& nums, int k, int t) {
        if (nums.empty()) {
            return false;
        }
        multiset<int64_t> q;
        for (int i = 0; i < nums.size(); ++i) {
            if (q.size() > k) {
                auto it = q.find(nums[i - k - 1]);
                if (it != end(q)) {
                    q.erase(it);
                }
            }
            q.insert(nums[i]);
            
            auto it = q.find(nums[i]);
            if (it != begin(q)) {
                if (abs(*it - *prev(it)) <= t) {
                    return true;
                }
            }
            if (next(it) != end(q)) {
                if (abs(*it - *next(it)) <= t) {
                    return true;
                }
            }
        }
        return false;
    }
};
