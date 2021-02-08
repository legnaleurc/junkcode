class Solution {
public:
    vector<vector<int>> insert(vector<vector<int>>& intervals, vector<int>& newInterval) {
        intervals.push_back(newInterval);
        sort(begin(intervals), end(intervals), [](
            const vector<int>& a,
            const vector<int>& b
        ) -> bool {
            return a[0] < b[0];
        });

        vector<vector<int>> rv;
        rv.push_back(intervals[0]);
        for (int i = 1; i < intervals.size(); ++i) {
            auto & last = rv.back();
            if (last[1] >= intervals[i][0]) {
                last[1] = max(last[1], intervals[i][1]);
            } else {
                rv.push_back(intervals[i]);
            }
        }
        return rv;
    }
};
