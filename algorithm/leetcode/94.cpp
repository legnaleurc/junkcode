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
    vector<int> inorderTraversal(TreeNode* root) {
        if (!root) {
            return {};
        }
        auto l = inorderTraversal(root->left);
        auto r = inorderTraversal(root->right);
        l.push_back(root->val);
        l.insert(l.end(), r.begin(), r.end());
        return l;
    }
};
