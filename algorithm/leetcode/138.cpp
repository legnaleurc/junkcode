/*
// Definition for a Node.
class Node {
public:
    int val;
    Node* next;
    Node* random;

    Node() {}

    Node(int _val, Node* _next, Node* _random) {
        val = _val;
        next = _next;
        random = _random;
    }
};
*/
class Solution {
public:
    Node* copyRandomList(Node* head) {
        if (!head) {
            return nullptr;
        }

        unordered_map<Node*, Node*> cache;
        auto newHead = new Node(head->val, nullptr, head->random);
        cache.insert({ head, newHead });

        auto a = head;
        auto b = newHead;
        while (a->next) {
            a = a->next;
            auto node = new Node(a->val, nullptr, a->random);
            cache.insert({ a, node });
            b->next = node;
            b = b->next;
        }

        b = newHead;
        while (b) {
            if (b->random) {
                b->random = cache[b->random];
            }
            b = b->next;
        }

        return newHead;
    }
};
