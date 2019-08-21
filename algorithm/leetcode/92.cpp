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
    ListNode* reverseBetween(ListNode* head, int m, int n) {
        ListNode* headTail = nullptr;
        auto midHead = head;
        for (int i = 1; i < m; ++i) {
            headTail = midHead;
            midHead = midHead->next;
        }
        auto midTail = midHead;
        for (int i = m; i < n; ++i) {
            midTail = midTail->next;
        }
        auto tailHead = midTail->next;

        reverseList(midHead, midTail);

        if (headTail) {
            headTail->next = midTail;
        } else {
            head = midTail;
        }
        midHead->next = tailHead;
        return head;
    }

    ListNode* reverseList(ListNode* head, ListNode* tail) {
        if (!head) {
            return nullptr;
        }
        if (!head->next) {
            return head;
        }
        if (head == tail) {
            return head;
        }
        auto newTail = head->next;
        auto newHead = reverseList(head->next, tail);
        newTail->next = head;
        head->next = nullptr;
        return newHead;
    }
};
