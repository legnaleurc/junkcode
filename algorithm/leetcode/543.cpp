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
    int diameterOfBinaryTree(TreeNode* root) {
        int rv = 0;
        getHeight(root, rv);
        return rv;
    }
    
    int getHeight(TreeNode* node, int& maxLength) {
        if (!node) {
            return 0;
        }
        if (!node->left && !node->right) {
            return 1;
        }
        auto l = getHeight(node->left, maxLength);
        auto r = getHeight(node->right, maxLength);
        maxLength = max(maxLength, l + r);
        return max(l, r) + 1;
    }
};
