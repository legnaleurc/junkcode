class Solution {
public:
    vector<vector<int>> combinationSum2(vector<int>& candidates, int target) {
        sort(begin(candidates), end(candidates));
        vector<vector<int>> rv;
        vector<int> v;
        findSum(rv, candidates, target, v, 0, 0);
        return rv;
    }
    
    void findSum(
        vector<vector<int>>& rv,
        const vector<int>& candidates,
        int target,
        vector<int>& v,
        int sum,
        int i
    ) {
        if (sum == target) {
            auto it = find(begin(rv), end(rv), v);
            if (it == end(rv)) {
                rv.push_back(v);
            }
            return;
        }
        if (i >= candidates.size()) {
            return;
        }
        for (; i < candidates.size(); ++i) {
            if (sum + candidates[i] > target) {
                break;
            }
            v.push_back(candidates[i]);
            findSum(rv, candidates, target, v, sum + candidates[i], i + 1);
            v.pop_back();
        }
    }
};
