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
    void flatten(TreeNode* root) {
        helper(root);
    }
    
    TreeNode* helper(TreeNode* root) {
        if (!root) {
            return nullptr;
        }
        if (!root->left && !root->right) {
            return root;
        }

        auto left = root->left;
        auto right = root->right;
        auto leftTail = helper(left);
        auto rightTail = helper(right);

        if (left) {
            root->left = nullptr;
            root->right = left;
        }
        if (leftTail) {
            leftTail->left = nullptr;
            leftTail->right = right;
        }
        if (rightTail) {
            rightTail->left = nullptr;
            rightTail->right = nullptr;
        }

        return rightTail ? rightTail : leftTail;
    }
};
