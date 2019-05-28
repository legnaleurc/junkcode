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
    TreeNode* increasingBST(TreeNode* root) {
        return leftMost(root);
    }

    TreeNode* leftMost(TreeNode* node) {
        if (!node) {
            return nullptr;
        }
        auto root = node;
        if (node->left) {
            auto left = leftMost(node->left);
            root = left;
            while (left->right) {
                left = left->right;
            }
            left->right = node;
            node->left = nullptr;
        }
        if (node->right) {
            auto right = leftMost(node->right);
            node->right = right;
        }
        return root;
    }
};
