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
    vector<int> rightSideView(TreeNode* root) {
        if (!root) {
            return {};
        }

        vector<int> v;
        queue<TreeNode*> q;

        q.push(root);
        while (!q.empty()) {
            v.push_back(q.back()->val);
            const auto l = q.size();
            for (int i = 0; i < l; ++i) {
                const auto n = q.front();
                q.pop();
                if (n->left) {
                    q.push(n->left);
                }
                if (n->right) {
                    q.push(n->right);
                }
            }
        }

        return v;
    }
};
