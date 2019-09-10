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
        vector<int> rv;
        stack<TreeNode*> track;
        track.push(root);
        while (!track.empty()) {
            auto node = track.top();
            track.pop();
            if (!node) {
                continue;
            }
            rv.push_back(node->val);
            track.push(node->right);
            track.push(node->left);
        }
        return rv;
    }
};
