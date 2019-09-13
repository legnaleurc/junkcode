class Solution {
public:
    string largestNumber(vector<int>& nums) {
        vector<string> slist;
        transform(begin(nums), end(nums), back_inserter(slist), [](int i) -> string {
            return to_string(i);
        });
        sort(begin(slist), end(slist), [](const string& a, const string& b) -> bool {
            return a + b < b + a;
        });
        ostringstream sout;
        for (auto it = rbegin(slist); it != rend(slist); ++it) {
            sout << *it;
        }
        string rv = sout.str();
        auto p = rv.find_first_not_of('0');
        if (p == string::npos) {
            return "0";
        } else {
            return rv.substr(p);
        }
    }
};
