class Node;
using NodeSP = shared_ptr<Node>;

class Node {
public:
    unordered_map<char, NodeSP> children;
    bool terminal;
    Node(): children(), terminal(false) {
    }
};

class Trie {
    NodeSP root;
public:
    /** Initialize your data structure here. */
    Trie(): root(make_shared<Node>()) {
    }

    /** Inserts a word into the trie. */
    void insert(string word) {
        auto node = root;
        for (auto c : word) {
            auto it = node->children.find(c);
            if (it == end(node->children)) {
                auto newNode = make_shared<Node>();
                node->children.insert({ c, newNode });
                node = newNode;
            } else {
                node = it->second;
            }
        }
        node->terminal = true;
    }

    /** Returns if the word is in the trie. */
    bool search(string word) {
        auto node = root;
        for (auto c : word) {
            auto it = node->children.find(c);
            if (it == end(node->children)) {
                return false;
            }
            node = it->second;
        }
        return node->terminal;
    }

    /** Returns if there is any word in the trie that starts with the given prefix. */
    bool startsWith(string prefix) {
        auto node = root;
        for (auto c : prefix) {
            auto it = node->children.find(c);
            if (it == end(node->children)) {
                return false;
            }
            node = it->second;
        }
        return true;
    }
};

/**
 * Your Trie object will be instantiated and called as such:
 * Trie* obj = new Trie();
 * obj->insert(word);
 * bool param_2 = obj->search(word);
 * bool param_3 = obj->startsWith(prefix);
 */
