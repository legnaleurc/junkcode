class Solution {
public:
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        sort(begin(candidates), end(candidates));

        vector<vector<int>> rv;
        vector<int> v;
        findSum(rv, target, candidates, 0, v, 0);

        return rv;
    }
    
    void findSum(vector<vector<int>>& rv, int target, const vector<int>& candidates, int i, vector<int>& v, int sum) {
        if (sum == target) {
            rv.push_back(v);
            return;
        }
        if (i >= candidates.size()) {
            return;
        }
        for (; i < candidates.size(); ++i) {
            sum += candidates[i];
            if (sum > target) {
                return;
            }
            v.push_back(candidates[i]);
            findSum(rv, target, candidates, i, v, sum);
            sum -= candidates[i];
            v.pop_back();
        }
    }
};
