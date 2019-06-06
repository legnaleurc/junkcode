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
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
        if (!l1) {
            return l2;
        }
        if (!l2) {
            return l1;
        }
        if (l1->val == l2->val) {
            auto nextL1 = l1->next;
            auto nextL2 = l2->next;
            auto head = l1;
            auto tail = l2;
            l1->next = l2;
            tail->next = mergeTwoLists(nextL1, nextL2);
            return head;
        }
        if (l1->val < l2->val) {
            auto head = l1;
            auto nextL1 = l1->next;
            head->next = mergeTwoLists(nextL1, l2);
            return head;
        }
        if (l1->val > l2->val) {
            auto head = l2;
            auto nextL2 = l2->next;
            head->next = mergeTwoLists(l1, nextL2);
            return head;
        }
        return nullptr;
    }
};
