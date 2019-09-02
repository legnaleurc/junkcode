class Solution {
public:
    int numSquares(int n) {
        if (n < 1) {
            return 0;
        }
        vector<int> cache(n + 1, 0);
        iota(begin(cache), end(cache), 0);
        for (int i = 2; i <= n; ++i) {
            int k = floor(sqrt(i));
            for (int j = 1; j <= k; ++j) {
                cache[i] = min(cache[i], cache[i - j * j] + 1);
            }
        }
        return cache[n];
    }
};
