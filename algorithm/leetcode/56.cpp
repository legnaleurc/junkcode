class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        if (intervals.empty()) {
            return {};
        }
        sort(begin(intervals), end(intervals), [](const vector<int>& a, const vector<int>& b) -> bool {
            return a[0] < b[0];
        });
        vector<vector<int>> rv;
        rv.push_back(intervals[0]);
        for (int i = 1; i < intervals.size(); ++i) {
            auto & last = rv.back();
            const auto & v = intervals[i];
            if (last[1] >= v[0]) {
                last[1] = max(last[1], v[1]);
            } else {
                rv.push_back(v);
            }
        }
        return rv;
    }
};
