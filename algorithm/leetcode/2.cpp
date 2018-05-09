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
        ListNode* head = nullptr;
        ListNode* tail = nullptr;
        int carry = 0;
        while (true) {
            ListNode* n = nullptr;
            if (l1 && l2) {
                n = addTwo(l1, l2, carry);
                l1 = l1->next;
                l2 = l2->next;
            } else if (!l1 && !l2) {
                break;
            } else {
                ListNode* t = nullptr;
                auto l = l1 ? l1 : l2;
                n = addOne(l, carry, &t);
                l1 = nullptr;
                l2 = nullptr;
                tail->next = n;
                tail = t;
                break;
            }

            if (!head) {
                head = n;
                tail = n;
            } else {
                tail->next = n;
                tail = n;
            }
        }

        if (carry > 0) {
            tail->next = new ListNode(carry);
        }

        return head;
    }

    ListNode* addTwo(ListNode* l1, ListNode* l2, int& carry) {
        auto v = l1->val + l2->val + carry;
        carry = v / 10;
        return new ListNode(v % 10);
    }

    ListNode* addOne(ListNode* l, int& carry, ListNode** t) {
        ListNode* head = l;
        while (l) {
            l->val += carry;
            carry = l->val / 10;
            l->val = l->val % 10;
            *t = l;
            l = l->next;
        }
        return head;
    }
};
