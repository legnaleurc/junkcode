class Solution {
public:
    string minWindow(string s, string t) {
        unordered_map<char, int> window;
        for (char c : t) {
            auto it = window.find(c);
            if (it == window.end()) {
                window.insert({ c, -1 });
            } else {
                it->second -= 1;
            }
        }

        int rvL = 0;
        int rvR = 0;
        int l = 0;
        int r = 0;
        while (r <= s.size() && l <= r) {
            if (!isMatched(window)) {
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

    bool isMatched(const unordered_map<char, int>& window) {
        for (const auto & w : window) {
            if (w.second < 0) {
                return false;
            }
        }
        return true;
    }
};
