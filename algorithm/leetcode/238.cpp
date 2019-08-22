class Solution {
public:
    vector<int> productExceptSelf(vector<int>& nums) {
        if (nums.empty()) {
            return {};
        }

        vector<int> rv;

        vector<int> left({ 1 });
        for (int i = 0; i < nums.size() - 1; ++i) {
            left.push_back(left.back() * nums[i]);
        }
        vector<int> right({ 1 });
        for (int i = nums.size() - 1; i > 0; --i) {
            right.push_back(right.back() * nums[i]);
        }
        reverse(begin(right), end(right));

        for (int i = 0; i < nums.size(); ++i) {
            rv.push_back(left[i] * right[i]);
        }

        return rv;
    }
};
