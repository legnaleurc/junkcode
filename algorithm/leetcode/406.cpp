class Solution {
public:
    vector<pair<int, int>> reconstructQueue(vector<pair<int, int>>& people) {
        using P = pair<int, int>;
        
        sort(begin(people), end(people), [](const P & l, const P & r) -> bool {
            if (l.first > r.first) {
                return true;
            }
            if (l.first < r.first) {
                return false;
            }
            return l.second < r.second;
        });
        
        list<P> rv;
        for (const auto & p: people) {
            auto it = begin(rv);
            advance(it, p.second);
            rv.insert(it, p);
        }
        
        return { begin(rv), end(rv) };
    }
};
