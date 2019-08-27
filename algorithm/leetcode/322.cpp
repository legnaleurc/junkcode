class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        vector<int> cache(amount + 1, -1);
        cache[0] = 0;
        for (int i = 1; i <= amount; ++i) {
            for (auto coin : coins) {
                if (i < coin) {
                    continue;
                }
                if (cache[i-coin] < 0) {
                    continue;
                }
                if (cache[i] <= 0) {
                    cache[i] = cache[i-coin] + 1;
                } else {
                    cache[i] = min(cache[i], cache[i-coin] + 1);
                }
            }
        }
        return cache.back();
    }
};
