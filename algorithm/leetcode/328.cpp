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
    ListNode* oddEvenList(ListNode* head) {
        if (!head) {
            return nullptr;
        }
        if (!head->next) {
            return head;
        }

        auto oddHead = head;
        auto oddTail = oddHead;
        auto evenHead = head->next;
        auto evenTail = evenHead;
        head = evenTail->next;
        while (head) {
            oddTail->next = head;
            oddTail = oddTail->next;
            if (!head->next) {
                break;
            }
            head = head->next;
            evenTail->next = head;
            evenTail = evenTail->next;

            head = head->next;
        }

        oddTail->next = evenHead;
        evenTail->next = nullptr;
        return oddHead;
    }
};
