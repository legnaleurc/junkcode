class Solution {
public:
    int findKthLargest(vector<int>& nums, int k) {
        vector<int> q;
        auto cmp = greater<int>();
        for (auto i : nums) {
            q.push_back(i);
            push_heap(begin(q), end(q), cmp);
            if (q.size() > k) {
                pop_heap(begin(q), end(q), cmp);
                q.pop_back();
            }
        }
        return q.front();
    }
};
