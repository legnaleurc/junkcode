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
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        auto f = head;
        auto l = head;
        for (int i = 0; i < n; ++i) {
            l = l->next;
        }
        if (!l) {
            // remove the first
            return f->next;
        }
        while (l->next) {
            f = f->next;
            l = l->next;
        }
        f->next = f->next->next;
        return head;
    }
};
