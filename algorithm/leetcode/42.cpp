class Solution {
public:
    int trap(vector<int>& height) {
        const int N = height.size();
        if (N < 3) {
            return 0;
        }

        vector<int> leftTallest(N, 0);
        int tallest = 0;
        for (int i = 0; i < N; ++i) {
            tallest = max(tallest, height[i]);
            leftTallest[i] = tallest;
        }

        vector<int> rightTallest(N, 0);
        tallest = 0;
        for (int i = N - 1; i >= 0; --i) {
            tallest = max(tallest, height[i]);
            rightTallest[i] = tallest;
        }

        int rv = 0;
        for (int i = 1; i < N - 1; ++i) {
            int left = leftTallest[i-1];
            int right = rightTallest[i+1];
            int mid = height[i];
            if (left > mid && mid < right) {
                rv += (min(left, right) - mid);
            }
        }
        return rv;
    }
};
