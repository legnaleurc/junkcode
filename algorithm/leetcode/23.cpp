/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */

namespace std {
    template<>
    struct greater<ListNode*> {
        bool operator() (const ListNode* a, const ListNode* b) const {
            return a->val > b->val;
        }
    };
}

class Solution {
public:
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        priority_queue<ListNode*, vector<ListNode*>, greater<ListNode*>> q;
        for (auto head : lists) {
            if (head) {
                q.push(head);
            }
        }
        ListNode* head = nullptr;
        ListNode* tail = nullptr;
        while (!q.empty()) {
            auto node = q.top();
            q.pop();
            if (!head) {
                head = node;
                tail = node;
            } else {
                tail->next = node;
                tail = tail->next;
            }
            node = node->next;
            if (node) {
                q.push(node);
            }
        }
        return head;
    }
};
