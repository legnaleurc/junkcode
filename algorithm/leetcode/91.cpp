class Solution {
public:
    int numDecodings(string s) {
        if (s.size() == 0) {
            return 0;
        }
        if (s.size() == 1) {
            return s[0] == '0' ? 0 : 1;
        }

        int i = s.size() - 1;
        int twoAfter = 0;
        if (s[i] != '0') {
            twoAfter = 1;
        }
        --i;
        int oneAfter = 0;
        if (s[i] == '0') {
            oneAfter = 0;
        } else if (s[i] == '1') {
            oneAfter = 1 + twoAfter;
        } else if (s[i] == '2') {
            if (s[i+1] >= '0' && s[i+1] <= '6') {
                oneAfter = 1 + twoAfter;
            } else {
                oneAfter = twoAfter;
            }
        } else {
            oneAfter = twoAfter;
        }
        --i;

        for (; i >= 0; --i) {
            if (s[i] == '1') {
                int current = oneAfter + twoAfter;
                twoAfter = oneAfter;
                oneAfter = current;
            } else if (s[i] == '2') {
                int current = 0;
                if (s[i+1] >= '0' && s[i+1] <= '6') {
                    current = oneAfter + twoAfter;
                } else {
                    current = oneAfter;
                }
                twoAfter = oneAfter;
                oneAfter = current;
            } else if (s[i] == '0') {
                int current = 0;
                twoAfter = oneAfter;
                oneAfter = current;
            } else {
                twoAfter = oneAfter;
            }

        }
        return oneAfter;
    }
};
