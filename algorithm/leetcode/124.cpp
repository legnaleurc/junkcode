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
    int maxPathSum(TreeNode* root) {
        auto rv = traverse(root);
        return max({
            get<0>(rv),
            get<1>(rv),
        });
    }

    // 0: p|l-p|p-r, 1: l-p-r|l|r
    tuple<int, int>
    traverse(TreeNode* node) {
        if (node->left && node->right) {
            auto leftRV = traverse(node->left);
            auto rightRV = traverse(node->right);
            // can connect with parent
            int connect = max({
                // this node only
                node->val,
                // l-p
                node->val + get<0>(leftRV),
                // p-r
                node->val + get<0>(rightRV),
            });
            // cannot connect with parent
            int isolate = max({
                // l-p-r
                node->val + get<0>(leftRV) + get<0>(rightRV),
                // l
                get<0>(leftRV),
                get<1>(leftRV),
                // r
                get<0>(rightRV),
                get<1>(rightRV),
            });
            return { connect, isolate };
        } else if (node->left) {
            auto leftRV = traverse(node->left);
            // can connect with parent
            int connect = max({
                // this node only
                node->val,
                // l-p
                node->val + get<0>(leftRV),
            });
            // cannot connect with parent
            int isolate = max({
                // l
                get<0>(leftRV),
                get<1>(leftRV),
            });
            return { connect, isolate };
        } else if (node->right) {
            auto rightRV = traverse(node->right);
            // can connect with parent
            int connect = max({
                // this node only
                node->val,
                // p-r
                node->val + get<0>(rightRV),
            });
            // cannot connect with parent
            int isolate = max({
                // r
                get<0>(rightRV),
                get<1>(rightRV),
            });
            return { connect, isolate };
        } else {
            return { node->val, node->val };
        }
    }
};
