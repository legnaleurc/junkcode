/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Codec {
public:

    // Encodes a tree to a single string.
    string serialize(TreeNode* root) {
        ostringstream sout;
        serialize(sout, root);
        return sout.str();
    }

    // Decodes your encoded data to tree.
    TreeNode* deserialize(string data) {
        istringstream sin(data);
        return deserialize(sin);
    }
    
private:
    void serialize(ostream& out, TreeNode* node) {
        if (!node) {
            out << " null";
            return;
        }
        out << ' ' << node->val;
        serialize(out, node->left);
        serialize(out, node->right);
    }

    TreeNode* deserialize(istream& in) {
        string token;
        in >> token;
        if (token.empty() || token == "null") {
            return nullptr;
        }
        auto node = new TreeNode(stoi(token));
        node->left = deserialize(in);
        node->right = deserialize(in);
        return node;
    }
};

// Your Codec object will be instantiated and called as such:
// Codec codec;
// codec.deserialize(codec.serialize(root));
