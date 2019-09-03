class Solution {
public:
    string reverseVowels(string s) {
        int l = 0;
        int r = s.size() - 1;
        while (true) {
            while (l < r && !isVowel(s[l])) {
                ++l;
            }
            while (l < r && !isVowel(s[r])) {
                --r;
            }
            if (l >= r) {
                break;
            }
            if (!isVowel(s[l]) || !isVowel(s[r])) {
                break;
            }
            swap(s[l], s[r]);
            ++l;
            --r;
        }
        return s;
    }
    
    bool isVowel(char c) const {
        return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U';
    }
};
