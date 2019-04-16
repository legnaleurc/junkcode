class Solution {
public:
    vector<string> generateParenthesis(int n) {
        vector<string> rv;
        dfs(rv, "", n, 0, 0);
        return rv;
    }

    void dfs(vector<string> & rv, const string & s, int n, int l, int r) {
        if (r == n) {
            rv.push_back(s);
            return;
        }
        if (l < n) {
            dfs(rv, s + "(", n, l + 1, r);
        }
        if (r < l) {
            dfs(rv, s + ")", n, l, r + 1);
        }
    }
};
