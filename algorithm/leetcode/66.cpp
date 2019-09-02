class Solution {
public:
    vector<int> plusOne(vector<int>& digits) {
        if (digits.empty()) {
            return digits;
        }
        int carry = 0;
        digits.back() += 1;
        for (auto it = rbegin(digits); it != rend(digits); ++it) {
            int s = *it + carry;
            carry = s / 10;
            *it = s % 10;
        }
        if (carry > 0) {
            digits.insert(begin(digits), 1);
        }
        return digits;
    }
};
