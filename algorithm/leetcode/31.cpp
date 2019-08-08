class Solution {
public:
    void nextPermutation(vector<int>& nums) {
        nextPermutation(begin(nums), end(nums));
    }
    
    bool nextPermutation(vector<int>::iterator b, vector<int>::iterator e) {
        if (b == e) {
            return false;
        }
        if (distance(b, e) == 1) {
            return false;
        }
        auto b1 = next(b, 1);
        auto rv = nextPermutation(b1, e);
        if (rv) {
            return rv;
        }
        auto e1 = prev(e, 1);
        if (*b >= *e1) {
            reverse(b1, e);
            reverse(b, e);
            return false;
        }
        auto m = upper_bound(b1, e, *b);
        iter_swap(b, m);
        return true;
    }
};
