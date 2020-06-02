class Solution {
public:
    vector<int> findAnagrams(string s, string p) {
        if (s.size() < p.size()) {
            return {};
        }
        unordered_map<char, int> pp;
        for (auto c : p) {
            pushChar(pp, c);
        }
        unordered_map<char, int> ss;
        for (int i = 0; i < p.size(); ++i) {
            pushChar(ss, s[i]);
        }
        vector<int> rv;
        if (ss == pp) {
            rv.push_back(0);
        }
        for (int i = p.size(); i < s.size(); ++i) {
            pushChar(ss, s[i]);
            popChar(ss, s[i-p.size()]);
            if (ss == pp) {
                rv.push_back(i - p.size() + 1);
            }
        }
        return rv;
    }

    int getCode(char c) {
        int i = 1;
        int offset = c - 'a';
        return i << offset;
    }

    void pushChar(unordered_map<char, int>& m, char c) {
        auto it = m.find(c);
        if (it == m.end()) {
            m.insert({ c, 1 });
        } else {
            it->second += 1;
        }
    }

    void popChar(unordered_map<char, int>& m, char c) {
        auto it = m.find(c);
        if (it == m.end()) {
            return;
        }
        if (it->second <= 1) {
            m.erase(it);
            return;
        }
        it->second -= 1;
    }
};
