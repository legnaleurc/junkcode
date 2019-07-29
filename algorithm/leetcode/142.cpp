/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    ListNode *detectCycle(ListNode *head) {
        set<ListNode*> cache;
        while (head) {
            auto it = cache.find(head);
            if (it == end(cache)) {
                cache.insert(head);
            } else {
                return head;
            }
            head = head->next;
        }
        return nullptr;
    }
};
