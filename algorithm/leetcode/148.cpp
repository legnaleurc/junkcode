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
    ListNode* sortList(ListNode* head) {
        return mergeSort(head);
    }
    
    ListNode* mergeSort(ListNode* head) {
        if (!head || !head->next) {
            return head;
        }
        auto mid = getMid(head);
        auto half = mid->next;
        mid->next = nullptr;
        head = mergeSort(head);
        half = mergeSort(half);
        return merge(head, half);
    }

    ListNode* getMid(ListNode* head) {
        auto slow = head;
        auto fast = head->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        return slow;
    }
    
    ListNode* merge(ListNode* l1, ListNode* l2) {
        if (!l1 && !l2) {
            return nullptr;
        }
        if (!l1) {
            return l2;
        }
        if (!l2) {
            return l1;
        }
        if (l1->val < l2->val) {
            l1->next = merge(l1->next, l2);
            return l1;
        } else {
            l2->next = merge(l1, l2->next);
            return l2;
        }
        return nullptr;
    }
};
