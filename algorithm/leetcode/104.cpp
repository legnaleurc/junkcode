/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    int maxDepth(TreeNode* root) {
        int rv = 0;
        traverse(root, 1, rv);
        return rv;
    }

    void traverse(TreeNode* node, int d, int& rv) {
        if (!node) {
            return;
        }
        if (d > rv) {
            rv = d;
        }
        traverse(node->left, d + 1, rv);
        traverse(node->right, d + 1, rv);
    }
};
