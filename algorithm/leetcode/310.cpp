class Solution {
public:
    vector<int> findMinHeightTrees(int n, vector<vector<int>>& edges) {
        unordered_multimap<int, int> graph;
        unordered_map<int, int> count;
        for (int i = 0; i < n; ++i) {
            count.insert({ i, 0 });
        }
        for (const auto & e : edges) {
            graph.insert({ e[0], e[1] });
            graph.insert({ e[1], e[0] });
            count[e[0]] += 1;
            count[e[1]] += 1;
        }

        vector<int> rv;
        auto leaves = getLeaves(count);
        while (!leaves.empty()) {
            rv = leaves;
            
            for (auto l : leaves) {
                count.erase(l);
                auto er = graph.equal_range(l);
                for (auto it = er.first; it != er.second; ++it) {
                    auto ti = count.find(it->second);
                    if (ti != end(count)) {
                        ti->second -= 1;
                    }
                }
            }
            leaves = getLeaves(count);
        }

        return rv;
    }

    vector<int> getLeaves(const unordered_map<int, int>& count) {
        vector<int> rv;
        for (const auto & p : count) {
            if (p.second <= 1) {
                rv.push_back(p.first);
            }
        }
        return rv;
    }
};
