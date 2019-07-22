// Below is the interface for Iterator, which is already defined for you.
// **DO NOT** modify the interface for Iterator.

class Iterator {
    struct Data;
    Data* data;
public:
    Iterator(const vector<int>& nums);
    Iterator(const Iterator& iter);
    virtual ~Iterator();
    // Returns the next element in the iteration.
    int next();
    // Returns true if the iteration has more elements.
    bool hasNext() const;
};


class PeekingIterator {
    Iterator iter;
    int cache;
    bool hasCache;
public:
    PeekingIterator(const vector<int>& nums)
        : iter(nums)
        , cache(0)
        , hasCache(false)
    {
        // Initialize any member here.
        // **DO NOT** save a copy of nums and manipulate it directly.
        // You should only use the Iterator interface methods.
    }

    // Returns the next element in the iteration without advancing the iterator.
    int peek() {
        if (this->hasCache) {
            return this->cache;
        }
        this->cache = this->iter.next();
        this->hasCache = true;
        return this->cache;
    }

    // hasNext() and next() should behave the same as in the Iterator interface.
    // Override them if needed.
    int next() {
        if (this->hasCache) {
            this->hasCache = false;
            return this->cache;
        }
        return this->iter.next();
    }

    bool hasNext() const {
        if (this->hasCache) {
            return true;
        }
        return this->iter.hasNext();
	}
};
