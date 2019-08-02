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
    vector<vector<int>> levelOrder(TreeNode* root) {
        vector<vector<int>> rv;
        traverse(rv, root, 0);
        return rv;
    }
    
    void traverse(vector<vector<int>>& rv, TreeNode* node, int depth) {
        if (!node) {
            return;
        }
        if (rv.size() <= depth) {
            rv.push_back({});
        }
        auto & level = rv[depth];
        level.push_back(node->val);
        traverse(rv, node->left, depth + 1);
        traverse(rv, node->right, depth + 1);
    }
};
