class Solution {
public:
    int minAddToMakeValid(string S) {
        int stack_ = 0;
        int rv = 0;
        for (auto c : S) {
            if (c == '(') {
                ++stack_;
            } else if (c == ')') {
                if (stack_ > 0) {
                    --stack_;
                } else {
                    ++rv;
                }
            }
        }
        return rv + stack_;
    }
};
