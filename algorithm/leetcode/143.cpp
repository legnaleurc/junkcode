/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    void reorderList(ListNode* head) {
        if (!head) {
            return;
        }

        auto slow = head;
        auto fast = head;
        while (slow && fast && fast->next && fast->next->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        auto reverseHead = slow->next;
        slow->next = nullptr;
        reverseHead = reverseList(reverseHead);

        while (head && reverseHead) {
            auto nextHead = head->next;
            auto nextReverseHead = reverseHead->next;
            
            head->next = reverseHead;
            reverseHead->next = nextHead;
            
            head = nextHead;
            reverseHead = nextReverseHead;
        }
    }

    ListNode* reverseList(ListNode* head) {
        if (!head) {
            return nullptr;
        }
        if (!head->next) {
            return head;
        }
        auto tail = head->next;
        auto newHead = reverseList(tail);
        tail->next = head;
        head->next = nullptr;
        return newHead;
    }
};
