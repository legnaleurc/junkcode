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
    vector<ListNode*> splitListToParts(ListNode* root, int k) {
        const int totalLength = getListLength(root);
        const int chunkSize = totalLength / k;
        int restSize = totalLength % k;
        vector<ListNode*> rv;
        auto head = root;
        for (int i = 0; i < k; ++i) {
            int size = chunkSize;
            if (restSize > 0) {
                size += 1;
                restSize -= 1;
            }
            auto nextHead = getSubList(head, size);
            rv.push_back(head);
            head = nextHead;
        }
        return rv;
    }
    
    int getListLength(ListNode* node) {
        int l = 0;
        while (node) {
            node = node->next;
            ++l;
        }
        return l;
    }
    
    ListNode* getSubList(ListNode* node, int k) {
        if (!node) {
            return nullptr;
        }
        if (k == 0) {
            return nullptr;
        }
        auto tail = node;
        for (int i = 0; i < k - 1; ++i) {
            tail = tail->next;
        }
        node = tail->next;
        tail->next = nullptr;
        return node;
    }
};
