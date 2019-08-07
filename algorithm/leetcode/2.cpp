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
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        int carry = 0;
        auto head = addTwoNumbers(l1, l2, carry);
        auto tail = head;
        while (tail) {
            if (l1) {
                l1 = l1->next;
            }
            if (l2) {
                l2 = l2->next;
            }
            auto node = addTwoNumbers(l1, l2, carry);
            tail->next = node;
            tail = tail->next;
        }
        return head;
    }

    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2, int& carry) {
        if (!l1 && !l2) {
            if (carry <= 0) {
                return nullptr;
            }
        } else if (!l1) {
            if (carry <= 0) {
                return l2;
            }
            carry += l2->val;
        } else if (!l2) {
            if (carry <= 0) {
                return l1;
            }
            carry += l1->val;
        } else {
            carry += l1->val + l2->val;
        }
        auto node = new ListNode(carry % 10);
        carry /= 10;
        return node;
    }
};
