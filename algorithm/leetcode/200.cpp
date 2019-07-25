template<typename T>
struct Node {
    Node(const T & v): value(v), parent() {}
    T value;
    weak_ptr<Node<T>> parent;
};

template<typename T>
class Set {
public:
    void union_(const T & a, const T & b) {
        auto x = this->find(a);
        auto y = this->find(b);
        x->parent = y;
    }
    
    shared_ptr<Node<T>> find(const T & a) {
        auto node = this->make(a);
        auto parent = node->parent.lock();
        if (parent == node) {
            return node;
        } else {
            return this->find(parent->value);
        }
    }
    
    shared_ptr<Node<T>> make(const T & a) {
        auto it = this->dict_.find(a);
        if (it == this->dict_.end()) {
            auto node = make_shared<Node<T>>(a);
            node->parent = node;
            this->dict_.insert({a, node});
            return node;
        }
        return it->second;
    }
    
    int rootCount() const {
        int sum = 0;
        for (auto it = begin(this->dict_); it != end(this->dict_); ++it) {
            auto node = it->second;
            auto parent = node->parent.lock();
            if (node == parent) {
                ++sum;
            }
        }
        return sum;
    }
private:
    map<T, shared_ptr<Node<T>>> dict_;
};

namespace std {
    template<>
    struct less<tuple<int, int>> {
        bool
        operator()(const tuple<int, int>& lhs, const tuple<int, int>& rhs) const
        {
            if (get<0>(lhs) < get<0>(rhs)) {
                return true;
            }
            if (get<0>(lhs) > get<0>(rhs)) {
                return false;
            }
            if (get<1>(lhs) < get<1>(rhs)) {
                return true;
            }
            return false;
        }
    };
}

class Solution {
public:
    int numIslands(vector<vector<char>>& grid) {
        Set<tuple<int, int>> group;
        for (int row = 0; row < grid.size(); ++row) {
            auto & flow = grid[row];
            for (int col = 0; col < flow.size(); ++col) {
                auto c = flow[col];
                if (c != '1') {
                    continue;
                }
                group.make({row, col});
                if (col > 0) {
                    if (flow[col-1] == '1') {
                        group.union_({row, col}, {row, col - 1});
                    }
                }
                if (row > 0) {
                    if (grid[row-1][col] == '1') {
                        group.union_({row, col}, {row - 1, col});
                    }
                }
            }
        }
        return group.rootCount();
    }
};
