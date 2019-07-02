class Solution {
public:
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        sort(begin(candidates), end(candidates));

        vector<vector<int>> rv;
        for (int i = 0; i < candidates.size(); ++i) {
            findSum(rv, target, candidates, i, {}, 0);
        }

        return rv;
    }
    
    void findSum(vector<vector<int>>& rv, int target, const vector<int>& candidates, int i, vector<int> v, int sum) {
        if (i >= candidates.size()) {
            return;
        }
        sum += candidates[i];
        if (sum > target) {
            return;
        }
        v.push_back(candidates[i]);
        if (sum == target) {
            rv.push_back(v);
            return;
        }
        for (; i < candidates.size(); ++i) {
            findSum(rv, target, candidates, i, v, sum);
        }
    }
};
