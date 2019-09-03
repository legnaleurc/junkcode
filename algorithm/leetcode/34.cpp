class Solution {
public:
    vector<int> searchRange(vector<int>& nums, int target) {
        auto p = equal_range(begin(nums), end(nums), target);
        if (p.first == p.second) {
            return { -1, -1 };
        }
        return { distance(begin(nums), p.first), distance(begin(nums), p.second) - 1 };
    }
};
