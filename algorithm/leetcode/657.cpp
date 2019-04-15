class Solution {
public:
    bool judgeCircle(string moves) {
        int v = 0;
        int h = 0;
        for (auto c : moves) {
            switch (c) {
                case 'R':
                    h -= 1;
                    break;
                case 'L':
                    h += 1;
                    break;
                case 'U':
                    v += 1;
                    break;
                case 'D':
                    v -= 1;
                    break;
            }
        }
        return v == 0 && h == 0;
    }
};
