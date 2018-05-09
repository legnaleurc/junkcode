/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    TreeNode* sortedArrayToBST(vector<int>& nums) {
        return part(nums, 0, nums.size());
    }

    TreeNode* part(const vector<int> & nums, int b, int e) {
        if (b == e) {
            return nullptr;
        }
        int m = b + (e - b) / 2;
        auto r = new TreeNode(nums[m]);
        r->left = part(nums, b, m);
        r->right = part(nums, m + 1, e);
        return r;
    }
};
