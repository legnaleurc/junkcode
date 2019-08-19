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
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        if (preorder.size() != inorder.size()) {
            return nullptr;
        }
        if (preorder.empty()) {
            return nullptr;
        }
        return createRoot(preorder, 0, preorder.size(), inorder, 0, inorder.size());
    }
    
    TreeNode* createRoot(const vector<int>& preorder,
                         int preBegin,
                         int preEnd,
                         const vector<int>& inorder,
                         int inBegin,
                         int inEnd)
    {
        if (preBegin >= preEnd) {
            return nullptr;
        }

        auto root = new TreeNode(preorder[preBegin]);
        if (preBegin == preEnd - 1) {
            return root;
        }

        int inPivot = findPivot(inorder, inBegin, inEnd, preorder[preBegin]);
        int prePivot = preBegin + 1 + (inPivot - inBegin);

        root->left = createRoot(preorder, preBegin + 1, prePivot, inorder, inBegin, inPivot);
        root->right = createRoot(preorder, prePivot, preEnd, inorder, inPivot + 1, inEnd);

        return root;
    }

    int findPivot(const vector<int>& v,
                  int b,
                  int e,
                  int p)
    {
        auto i = begin(v);
        auto bb = next(i, b);
        auto ee = next(i, e);
        auto it = find(bb, ee, p);
        return distance(i, it);
    }
};
