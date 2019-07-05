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
    int kthSmallest(TreeNode* root, int k) {
        vector<int> rv;
        traverse(rv, root);
        return rv[k-1];
    }
    
    void traverse(vector<int>& rv, TreeNode* node) {
        if (!node) {
            return;
        }
        traverse(rv, node->left);
        rv.push_back(node->val);
        traverse(rv, node->right);
    }
};
