class Solution {
public:
    int findLongestChain(vector<vector<int>>& pairs) {
        if (pairs.empty()) {
            return 0;
        }

        sort(begin(pairs), end(pairs), [](const vector<int> & a, const vector<int> & b) -> bool {
            return a[1] < b[1];
        });

        int rv = 1;
        int end = pairs[0][1];
        for (int i = 1; i < pairs.size(); ++i) {
            if (end >= pairs[i][0]) {
                continue;
            }
            end = pairs[i][1];
            ++rv;
        }

        return rv;
    }
};
