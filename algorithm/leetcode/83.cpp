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
    ListNode* deleteDuplicates(ListNode* head) {
        auto now = head;
        while (now) {
            auto nextNode = now->next;
            while (nextNode && now->val == nextNode->val) {
                now->next = nextNode->next;
                nextNode = now->next;
            }
            now = now->next;
        }
        return head;
    }
};
