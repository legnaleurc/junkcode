/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class CBTInserter {
public:
    CBTInserter(TreeNode* root): root_(root) {
        assert(root);
        q_.push(root);
    }

    int insert(int v) {
        assert(root_);
        findNext();
        auto n = new TreeNode(v);
        auto p = q_.front();
        if (!p->left) {
            p->left = n;
        } else {
            p->right = n;
        }
        return p->val;
    }
    
    TreeNode* get_root() {
        return root_;
    }
private:
    void findNext() {
        while (!q_.empty()) {
            auto node = q_.front();
            if (!node->left || !node->right) {
                break;
            }
            q_.pop();
            q_.push(node->left);
            q_.push(node->right);
        }
    }

    TreeNode* root_;
    std::queue<TreeNode*> q_;
};

/**
 * Your CBTInserter object will be instantiated and called as such:
 * CBTInserter* obj = new CBTInserter(root);
 * int param_1 = obj->insert(v);
 * TreeNode* param_2 = obj->get_root();
 */
