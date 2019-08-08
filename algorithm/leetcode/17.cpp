class Solution {
public:
    vector<string> letterCombinations(string digits) {
        if (digits.empty()) {
            return {};
        }
        unordered_map<char, vector<char>> mapping{{
            { '2', { 'a', 'b', 'c' } },
            { '3', { 'd', 'e', 'f' } },
            { '4', { 'g', 'h', 'i' } },
            { '5', { 'j', 'k', 'l' } },
            { '6', { 'm', 'n', 'o' } },
            { '7', { 'p', 'q', 'r', 's' } },
            { '8', { 't', 'u', 'v' } },
            { '9', { 'w', 'x', 'y', 'z' } },
        }};
        vector<string> rv;
        string buffer = digits;
        traverse(rv, mapping, digits, 0, buffer);
        return rv;
    }
    
    void traverse(vector<string>& rv, const unordered_map<char, vector<char>>& mapping, const string& digits, int start, string& buffer) {
        if (start >= digits.size()) {
            rv.push_back(buffer);
            return;
        }
        auto it = mapping.find(digits[start]);
        const auto & alphas = it->second;
        for (auto alpha : alphas) {
            buffer[start] = alpha;
            traverse(rv, mapping, digits, start + 1, buffer);
        }
    }
};
