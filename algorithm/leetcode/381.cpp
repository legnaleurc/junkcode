class RandomizedCollection {
    vector<int> v;
    unordered_multimap<int, int> umm;
public:
    /** Initialize your data structure here. */
    RandomizedCollection() {
        srand(time(nullptr));
    }
    
    /** Inserts a value to the collection. Returns true if the collection did not already contain the specified element. */
    bool insert(int val) {
        auto it = umm.find(val);
        bool rv = it == end(umm);
        umm.insert({ val, v.size() });
        v.push_back(val);
        return rv;
    }
    
    /** Removes a value from the collection. Returns true if the collection contained the specified element. */
    bool remove(int val) {
        auto it = umm.find(val);
        if (it == end(umm)) {
            return false;
        }
        int index = it->second;
        int last = v.size() - 1;
        if (index != last) {
            swap(v[index], v[last]);
            auto eq = umm.equal_range(v[index]);
            for (auto ti = eq.first; ti != eq.second; ++ti) {
                if (ti->second == last) {
                    ti->second = index;
                }
            }
        }
        v.pop_back();
        umm.erase(it);
        return true;
    }
    
    /** Get a random element from the collection. */
    int getRandom() {
        int index = rand() / ((RAND_MAX + 1u) / v.size());
        return v[index];
    }
};

/**
 * Your RandomizedCollection object will be instantiated and called as such:
 * RandomizedCollection* obj = new RandomizedCollection();
 * bool param_1 = obj->insert(val);
 * bool param_2 = obj->remove(val);
 * int param_3 = obj->getRandom();
 */
