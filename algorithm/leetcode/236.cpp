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
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        vector<TreeNode*> pp;
        getAncestors(pp, root, p);
        reverse(begin(pp), end(pp));

        vector<TreeNode*> qp;
        getAncestors(qp, root, q);
        reverse(begin(qp), end(qp));

        int i = 0;
        while (i < pp.size() && i < qp.size() && pp[i] == qp[i]) {
            ++i;
        }
        return pp[i-1];
    }

    TreeNode* getAncestors(vector<TreeNode*>& path, TreeNode* root, TreeNode* node) {
        if (!root) {
            return nullptr;
        }
        if (root == node) {
            path.push_back(root);
            return root;
        }
        if (auto rv = getAncestors(path, root->left, node)) {
            path.push_back(root);
            return rv;
        }
        if (auto rv = getAncestors(path, root->right, node)) {
            path.push_back(root);
            return rv;
        }
        return nullptr;
    }
};
