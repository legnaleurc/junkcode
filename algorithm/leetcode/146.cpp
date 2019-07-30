using Pair = pair<int, int>;
using PairList = list<Pair>;

bool cmp (const Pair & l, const Pair & r) {
    return l.second > r.second;
}

class LRUCache {
    PairList cycle;
    unordered_map<int, PairList::iterator> dict;
    int cap;
public:
    LRUCache(int capacity)
        : cycle()
        , dict()
        , cap(capacity)
    {
    }
    
    int get(int key) {
        auto it = dict.find(key);
        if (it == end(dict)) {
            return -1;
        }
        auto cache = it->second;
        auto rv = cache->second;
        cycle.erase(cache);
        cycle.push_front({ key, rv });
        it->second = begin(cycle);
        return rv;
    }
    
    void put(int key, int value) {
        auto it = dict.find(key);
        if (it != end(dict)) {
            auto cache = it->second;
            cycle.erase(cache);
            cycle.push_front({ key, value });
            it->second = begin(cycle);
        } else {
            if (cycle.size() >= cap) {
                auto last = prev(end(cycle));
                dict.erase(last->first);
                cycle.erase(last);
            }
            cycle.push_front({ key, value });
            dict.insert({ key, cycle.begin() });
        }
    }
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */
