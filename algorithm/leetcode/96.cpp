class Solution {
    unordered_map<int, int> cache;
public:
    Solution() {
        cache.insert({ 0, 1 });
    }

    int numTrees(int n) {
        if (n < 0) {
            return 0;
        }
        auto it = cache.find(n);
        if (it != end(cache)) {
            return it->second;
        }
        int sum = 0;
        for (int i = 0; i < n; ++i) {
            sum += numTrees(i) * numTrees(n - 1 - i);
        }
        cache.insert({ n, sum });
        return sum;
    }
};
