class Solution {
public:
    int maxProfit(vector<int>& prices) {
        if (prices.size() < 2) {
            return 0;
        }
        int rv = 0;
        int buy = prices[0];
        for (int i = 1; i < prices.size(); ++i) {
            int d = prices[i] - buy;
            if (d <= 0) {
                buy = prices[i];
            }
            rv = max(rv, d);
        }
        return rv;
    }
};
