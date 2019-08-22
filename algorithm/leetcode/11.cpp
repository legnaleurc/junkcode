class Solution {
public:
    int maxArea(vector<int>& height) {
        if (height.empty()) {
            return 0;
        }

        int l = 0;
        int r = height.size() - 1;
        int area = 0;

        while (l < r) {
            int h = min(height[l], height[r]);
            int w = r - l;
            int a = h * w;
            if (a > area) {
                area = a;
            }

            if (height[l] > height[r]) {
                --r;
            } else {
                ++l;
            }
        }

        return area;
    }
};
