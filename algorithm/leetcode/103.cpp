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
    vector<vector<int>> zigzagLevelOrder(TreeNode* root) {
        if (!root) {
            return {};
        }
        vector<vector<int>> rv;
        deque<TreeNode*> q;
        q.push_back(root);
        while (!q.empty()) {
            int l = q.size();
            vector<int> v;
            if (rv.size() % 2 == 0) {
                while (l--) {
                    auto node = q.front();
                    v.push_back(node->val);
                    q.pop_front();
                    if (node->left) {
                        q.push_back(node->left);
                    }
                    if (node->right) {
                        q.push_back(node->right);
                    }
                }
            } else {
                while (l--) {
                    auto node = q.back();
                    v.push_back(node->val);
                    q.pop_back();
                    if (node->right) {
                        q.push_front(node->right);
                    }
                    if (node->left) {
                        q.push_front(node->left);
                    }
                }
            }
            rv.push_back(v);
        }
        return rv;
    }
};
