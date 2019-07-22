/*
// Definition for a Node.
class Node {
public:
    int val;
    Node* prev;
    Node* next;
    Node* child;

    Node() {}

    Node(int _val, Node* _prev, Node* _next, Node* _child) {
        val = _val;
        prev = _prev;
        next = _next;
        child = _child;
    }
};
*/
class Solution {
public:
    Node* flatten(Node* head) {
        newTail(head);
        return head;
    }
    
    Node* newTail(Node* node) {
        auto tail = node;
        while (node) {
            if (!node->child) {
                tail = node;
                node = node->next;
            } else {
                auto last = newTail(node->child);

                auto next = node->next;
                node->child->prev = node;
                node->next = node->child;
                node->child = nullptr;

                last->next = next;
                if (next) {
                    next->prev = last;
                }

                node = last->next;
                tail = last;
                
            }
        }
        return tail;
    }
};
