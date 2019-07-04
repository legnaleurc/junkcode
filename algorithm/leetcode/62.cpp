class Solution {
    using Table = unordered_map<int, unordered_map<int, int>>;
public:
    int uniquePaths(int m, int n) {
        Table table;
        int rv = getPaths(table, m - 1, n - 1);
        return rv;
    }
    
    int getPaths(Table& table, int x, int y) {
        if (x == 0 || y == 0) {
            return 1;
        }
        if (x > y) {
            swap(x, y);
        }
        int rv = 0;
        bool ok = getCache(rv, table, x, y);
        if (ok) {
            return rv;
        }
        rv = getPaths(table, x - 1, y) + getPaths(table, x, y - 1);
        setCache(table, x, y, rv);
        return rv;
    }
    
    bool getCache(int& rv, const Table& table, int x, int y) {
        auto it1 = table.find(x);
        if (it1 == end(table)) {
            return false;
        }
        auto it2 = it1->second.find(y);
        if (it2 == end(it1->second)) {
            return false;
        }
        rv = it2->second;
        return true;
    }
    
    void setCache(Table& table, int x, int y, int rv) {
        auto it1 = table.find(x);
        if (it1 == end(table)) {
            it1 = table.insert(it1, make_pair(x, unordered_map<int, int>()));
        }
        auto it2 = it1->second.find(y);
        if (it2 == end(it1->second)) {
            it2 = it1->second.insert(it2, make_pair(y, rv));
        }
        it2->second = rv;
    }
};
