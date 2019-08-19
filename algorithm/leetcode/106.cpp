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
    TreeNode* buildTree(vector<int>& inorder, vector<int>& postorder) {
        return createRoot(inorder, 0, inorder.size(), postorder, 0, postorder.size());
    }

    TreeNode* createRoot(const vector<int>& inorder,
                         int inBegin,
                         int inEnd,
                         const vector<int>& postorder,
                         int postBegin,
                         int postEnd)
    {
        if (postBegin >= postEnd) {
            return nullptr;
        }

        auto root = new TreeNode(postorder[postEnd-1]);
        if (postEnd - postBegin == 1) {
            return root;
        }

        int inPivot = findPivot(inorder, inBegin, inEnd, postorder[postEnd-1]);
        int postPivot = postBegin + (inPivot - inBegin);

        root->left = createRoot(inorder, inBegin, inPivot, postorder, postBegin, postPivot);
        root->right = createRoot(inorder, inPivot + 1, inEnd, postorder, postPivot, postEnd - 1);

        return root;
    }

    int findPivot(const vector<int>& v, int b, int e, int p) {
        auto i = begin(v);
        auto bb = next(i, b);
        auto ee = next(i, e);
        auto it = find(bb, ee, p);
        return distance(i, it);
    }
};
