/*
// Definition for a Node.
class Node {
public:
    int val;
    vector<Node*> neighbors;
    Node() {
        val = 0;
        neighbors = vector<Node*>();
    }
    Node(int _val) {
        val = _val;
        neighbors = vector<Node*>();
    }
    Node(int _val, vector<Node*> _neighbors) {
        val = _val;
        neighbors = _neighbors;
    }
};
*/

class Solution {
public:
    Node* cloneGraph(Node* node) {
        if (!node) {
            return nullptr;
        }

        unordered_map<Node*, Node*> nodes;
        firstPass(nodes, node);
        auto newHead = nodes[node];

        set<Node*> cache;
        secondPass(nodes, cache, node);

        return newHead;
    }

    void firstPass(unordered_map<Node*, Node*>& nodes, Node* node) {
        auto it = nodes.find(node);
        if (it != nodes.end()) {
            return;
        }
        nodes.insert({ node, new Node(node->val) });
        for (auto child : node->neighbors) {
            firstPass(nodes, child);
        }
    }

    void secondPass(unordered_map<Node*, Node*>& nodes, set<Node*>& cache, Node* node) {
        if (cache.count(node) >= 1) {
            return;
        }
        cache.insert(node);
        auto newNode = nodes[node];
        for (auto child : node->neighbors) {
            auto newChild = nodes[child];
            newNode->neighbors.push_back(newChild);
            secondPass(nodes, cache, child);
        }
    }
};
