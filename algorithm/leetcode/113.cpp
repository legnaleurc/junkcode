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
    vector<vector<int>> pathSum(TreeNode* root, int sum) {
        vector<vector<int>> rv;
        vector<int> path;
        traverse(root, sum, path, rv);
        return rv;
    }

    void traverse(TreeNode* node, int target, vector<int>& path, vector<vector<int>>& rv) {
        if (!node) {
            return;
        }
        if (!node->left && !node->right) {
            if (node->val == target) {
                path.push_back(node->val);
                rv.push_back(path);
                path.pop_back();
            }
            return;
        }
        path.push_back(node->val);
        traverse(node->left, target - node->val, path, rv);
        traverse(node->right, target - node->val, path, rv);
        path.pop_back();
    }
};
