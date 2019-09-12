/*
// Definition for a Node.
class Node {
public:
    int val;
    Node* left;
    Node* right;
    Node* next;

    Node() {}

    Node(int _val, Node* _left, Node* _right, Node* _next) {
        val = _val;
        left = _left;
        right = _right;
        next = _next;
    }
};
*/
class Solution {
public:
    Node* connect(Node* root) {
        traverse(root, nullptr);
        return root;
    }
    
    void traverse(Node* node, Node* right) {
        if (!node) {
            return;
        }
        node->next = right;
        traverse(node->left, node->right);
        traverse(node->right, right ? right->left : nullptr);
    }
};
