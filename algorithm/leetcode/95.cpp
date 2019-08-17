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
    vector<TreeNode*> generateTrees(int n) {
        if (n <= 0) {
            return {};
        }
        return generate(1, n);
    }

    vector<TreeNode*> generate(int b, int e) {
        if (b > e) {
            return {};
        }
        vector<TreeNode*> rv;
        for (int i = b; i <= e; ++i) {
            auto leftList = generate(b, i - 1);
            auto rightList = generate(i + 1, e);
            if (leftList.empty() && rightList.empty()) {
                auto root = new TreeNode(i);
                rv.push_back(root);
            } else if (leftList.empty()) {
                for (auto node : rightList) {
                    auto root = new TreeNode(i);
                    root->right = node;
                    rv.push_back(root);
                }
            } else if (rightList.empty()) {
                for (auto node : leftList) {
                    auto root = new TreeNode(i);
                    root->left = node;
                    rv.push_back(root);
                }
            } else {
                for (auto l : leftList) {
                    for (auto r : rightList) {
                        auto root = new TreeNode(i);
                        root->left = l;
                        root->right = r;
                        rv.push_back(root);
                    }
                }
            }
        }
        return rv;
    }
};
