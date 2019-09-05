class RandomizedSet {
    vector<int> v;
    unordered_map<int, int> um;
public:
    /** Initialize your data structure here. */
    RandomizedSet() {
        srand(time(nullptr));
    }
    
    /** Inserts a value to the set. Returns true if the set did not already contain the specified element. */
    bool insert(int val) {
        auto it = um.find(val);
        if (it != end(um)) {
            return false;
        }
        um.insert({ val, v.size() });
        v.push_back(val);
        return true;
    }
    
    /** Removes a value from the set. Returns true if the set contained the specified element. */
    bool remove(int val) {
        auto it = um.find(val);
        if (it == end(um)) {
            return false;
        }
        int index = it->second;
        um.erase(it);
        it = um.find(v.back());
        if (it != end(um)) {
            it->second = index;
        }
        swap(v[index], v.back());
        v.erase(prev(v.end()));
        return true;
    }
    
    /** Get a random element from the set. */
    int getRandom() {
        int index = rand() / ((RAND_MAX + 1u) / v.size());
        return v[index];
    }
};

/**
 * Your RandomizedSet object will be instantiated and called as such:
 * RandomizedSet* obj = new RandomizedSet();
 * bool param_1 = obj->insert(val);
 * bool param_2 = obj->remove(val);
 * int param_3 = obj->getRandom();
 */
