/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    TreeNode* sortedListToBST(ListNode* head) {
        auto mid = getMiddleNode(head);
        if (!mid) {
            return nullptr;
        }
        auto root = new TreeNode(mid->val);
        if (mid != head) {
            root->left = sortedListToBST(head);
        }
        root->right = sortedListToBST(mid->next);
        return root;
    }
    
    ListNode* getMiddleNode(ListNode* head) {
        if (!head) {
            return nullptr;
        }
        if (!head->next) {
            return head;
        }
        if (!head->next->next) {
            auto mid = head->next;
            head->next = nullptr;
            return mid;
        }

        auto headTail = head;
        auto mid = head;
        auto tail = head;
        while (tail->next && tail->next->next) {
            headTail = mid;
            mid = mid->next;
            tail = tail->next->next;
        }
        headTail->next = nullptr;
        return mid;
    }
};
