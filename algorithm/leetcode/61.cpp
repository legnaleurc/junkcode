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
    ListNode* rotateRight(ListNode* head, int k) {
        if (!head) {
            return nullptr;
        }
        if (k < 1) {
            return head;
        }
        auto pointerHead = head;
        auto pointerTail = head;
        for (int i = 0; i < k; ++i) {
            if (pointerTail->next) {
                pointerTail = pointerTail->next;
            } else {
                const int size = i + 1;
                k = k % size;
                i = -1;
                pointerTail = head;
            }
        }
        while (pointerTail->next) {
            pointerHead = pointerHead->next;
            pointerTail = pointerTail->next;
        }
        pointerTail->next = head;
        head = pointerHead->next;
        pointerHead->next = nullptr;
        return head;
    }
};
