/**
 * // This is the interface that allows for creating nested lists.
 * // You should not implement it, or speculate about its implementation
 * class NestedInteger {
 *   public:
 *     // Return true if this NestedInteger holds a single integer, rather than a nested list.
 *     bool isInteger() const;
 *
 *     // Return the single integer that this NestedInteger holds, if it holds a single integer
 *     // The result is undefined if this NestedInteger holds a nested list
 *     int getInteger() const;
 *
 *     // Return the nested list that this NestedInteger holds, if it holds a nested list
 *     // The result is undefined if this NestedInteger holds a single integer
 *     const vector<NestedInteger> &getList() const;
 * };
 */
class NestedIterator {
    using Iterator = vector<NestedInteger>::const_iterator;
    using IteratorPair = pair<Iterator, Iterator>;
    stack<IteratorPair> s;
public:
    NestedIterator(vector<NestedInteger> &nestedList) {
        s.push({ begin(nestedList), end(nestedList) });
    }

    int next() {
        auto & p = s.top();
        auto rv = p.first->getInteger();
        ++p.first;
        return rv;
    }

    bool hasNext() {
        findNext();
        return !s.empty();
    }
    
    void findNext() {
        auto p = s.top();
        while (true) {
            if (p.first == p.second) {
                s.pop();
                if (s.empty()) {
                    break;
                }
                ++s.top().first;
            } else if (!p.first->isInteger()) {
                const auto & v = p.first->getList();
                s.push({ begin(v), end(v) });
            } else {
                break;
            }
            p = s.top();
        }
    }
};

/**
 * Your NestedIterator object will be instantiated and called as such:
 * NestedIterator i(nestedList);
 * while (i.hasNext()) cout << i.next();
 */
