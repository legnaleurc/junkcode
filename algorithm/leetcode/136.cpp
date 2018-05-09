class Solution {
public:
    int singleNumber(vector<int>& nums) {
        set<int> table;
        for (int i : nums) {
            auto it = table.find(i);
            if (it == table.end()) {
                table.insert(i);
            } else {
                table.erase(it);
            }
        }
        return *table.begin();
    }
};
