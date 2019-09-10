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
    vector<int> preorderTraversal(TreeNode* root) {
        if (!root) {
            return {};
        }
        auto left = preorderTraversal(root->left);
        auto right = preorderTraversal(root->right);
        vector<int> rv;
        rv.push_back(root->val);
        rv.insert(end(rv), begin(left), end(left));
        rv.insert(end(rv), begin(right), end(right));
        return rv;
    }
};
