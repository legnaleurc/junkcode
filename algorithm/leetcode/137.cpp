class Solution {
public:
    int singleNumber(vector<int>& nums) {
        // value:count table
        map<int, int> table;
        for (int i : nums) {
            auto it = table.find(i);
            if (it == table.end()) {
                table.insert({i, 1});
            } else if (it->second == 1) {
                it->second = 2;
            } else {
                table.erase(it);
            }
        }
        return table.begin()->first;
    }
};
