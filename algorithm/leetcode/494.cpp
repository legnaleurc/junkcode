class Solution {
public:
    int findTargetSumWays(vector<int>& nums, int S) {
        unordered_map<int, int> cache({
            { 0, 1 },
        });

        for (auto i : nums) {
            unordered_map<int, int> tmp;
            for (const auto & p : cache) {
                tmp[p.first + i] += p.second;
                tmp[p.first - i] += p.second;
            }
            swap(cache, tmp);
        }

        return cache[S];
    }
};
