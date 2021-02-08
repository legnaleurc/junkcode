class Solution {
public:
    string minWindow(string s, string t) {
        unordered_map<char, int> target;
        for (char c : t) {
            auto it = target.find(c);
            if (it == target.end()) {
                target.insert({ c, 1 });
            } else {
                it->second += 1;
            }
        }
        
        unordered_map<char, int> window;
        for (char c : t) {
            window.insert({ c, 0 });
        }

        int rvL = 0;
        int rvR = 0;
        int l = 0;
        int r = 0;
        while (r <= s.size() && l <= r) {
            if (!isMatched(window, target)) {
                ++r;
                addChar(window, s[r-1]);
                continue;
            }
            if (rvR == rvL || r - l < rvR - rvL) {
                rvL = l;
                rvR = r;
            }
            removeChar(window, s[l]);
            ++l;
        }
        return s.substr(rvL, rvR - rvL);
    }

    void addChar(unordered_map<char, int>& window, char c) {
        auto it = window.find(c);
        if (it != window.end()) {
            it->second += 1;
        }
    }

    void removeChar(unordered_map<char, int>& window, char c) {
        auto it = window.find(c);
        if (it != window.end()) {
            it->second -= 1;
        }
    }

    bool isMatched(const unordered_map<char, int>& window, const unordered_map<char, int>& target) {
        for (const auto t : target) {
            auto it = window.find(t.first);
            if (it->second < t.second) {
                return false;
            }
        }
        return true;
    }
};
