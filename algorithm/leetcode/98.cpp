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
    bool isValidBST(TreeNode* root) {
        return isValidBST(root, nullptr, nullptr);
    }

    bool isValidBST(TreeNode* node, TreeNode* minNode, TreeNode* maxNode) {
        if (!node) {
            return true;
        }
        if (minNode && node->val <= minNode->val) {
            return false;
        }
        if (maxNode && node->val >= maxNode->val) {
            return false;
        }
        if (!isValidBST(node->left, minNode, node)) {
            return false;
        }
        if (!isValidBST(node->right, node, maxNode)) {
            return false;
        }
        return true;
    }
};
