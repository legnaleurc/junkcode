class Node;
using NodeSP = shared_ptr<Node>;
class Node {
public:
    Node(): children(), terminal(false) {}
    unordered_map<char, NodeSP> children;
    bool terminal;
};

class WordDictionary {
    NodeSP root;
public:
    /** Initialize your data structure here. */
    WordDictionary(): root(make_shared<Node>()) {
    }
    
    void addWord(string word) {
        auto node = root;
        for (char c : word) {
            auto & children = node->children;
            auto it = children.find(c);
            if (it == children.end()) {
                auto newNode = make_shared<Node>();
                children.insert({ c, newNode });
                node = newNode;
            } else {
                node = it->second;
            }
        }
        node->terminal = true;
    }
    
    bool search(string word) {
        return search2(word, 0, root);
    }
    
    bool search2(const string& word, int i, NodeSP node) {
        if (i >= word.size()) {
            return node->terminal;
        }
        const auto & children = node->children;
        if (word[i] != '.') {
            auto it = children.find(word[i]);
            if (it == children.end()) {
                return false;
            }
            return search2(word, i + 1, it->second);
        }
        for (const auto & p : children) {
            if (search2(word, i + 1, p.second)) {
                return true;
            }
        }
        return false;
    }
};

/**
 * Your WordDictionary object will be instantiated and called as such:
 * WordDictionary* obj = new WordDictionary();
 * obj->addWord(word);
 * bool param_2 = obj->search(word);
 */
