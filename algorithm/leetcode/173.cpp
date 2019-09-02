/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class BSTIterator {
    stack<TreeNode*> s_;
public:
    BSTIterator(TreeNode* root) {
        while (root) {
            s_.push(root);
            root = root->left;
        }
    }
    
    /** @return the next smallest number */
    int next() {
        auto rv = s_.top()->val;
        auto root = s_.top()->right;
        s_.pop();
        while (root) {
            s_.push(root);
            root = root->left;
        }
        return rv;
    }
    
    /** @return whether we have a next smallest number */
    bool hasNext() {
        return !s_.empty();
    }
};

/**
 * Your BSTIterator object will be instantiated and called as such:
 * BSTIterator* obj = new BSTIterator(root);
 * int param_1 = obj->next();
 * bool param_2 = obj->hasNext();
 */
