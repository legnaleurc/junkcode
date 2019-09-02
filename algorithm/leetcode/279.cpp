class Solution {
public:
    int numSquares(int n) {
        if (n < 1) {
            return 0;
        }
        unordered_map<int, int> cache;
        return search(n, cache);
    }

    int search(int n, unordered_map<int, int>& cache) {
        if (n == 1) {
            return 1;
        }
        auto it = cache.find(n);
        if (it != end(cache)) {
            return it->second;
        }
        int rv = n;
        int r = floor(sqrt(n));
        while (r >= 1) {
            rv = min(rv, search(n - r * r, cache) + 1);
            --r;
        }
        cache.insert({ n, rv });
        return rv;
    }
};
