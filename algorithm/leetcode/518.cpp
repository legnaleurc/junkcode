class Solution {
public:
    int change(int amount, vector<int>& coins) {
        vector<int> cache(amount + 1, 0);
        cache[0] = 1;
        for (auto c : coins) {
            for (int i = c; i <= amount; ++i) {
                cache[i] += cache[i-c];
            }
        }
        return cache[amount];
    }
};
