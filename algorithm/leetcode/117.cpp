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
        auto tail = new Node(0, nullptr, nullptr, nullptr);
        auto tmp = tail;
        auto node = root;
        while (node) {
            tail->next = node->left;
            if (tail->next) {
                tail = tail->next;
            }
            tail->next = node->right;
            if (tail->next) {
                tail = tail->next;
            }
            node = node->next;
            if (!node) {
                tail = tmp;
                node = tmp->next;
            }
        }
        return root;
    }
};
