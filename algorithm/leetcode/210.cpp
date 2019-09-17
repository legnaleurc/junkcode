class Solution {
public:
    vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
        unordered_multimap<int, int> edges;
        unordered_map<int, int> counts;
        for (int i = 0; i < numCourses; ++i) {
            counts.insert({ i, 0 });
        }
        for (const auto & p : prerequisites) {
            edges.insert({ p[1], p[0] });
            counts[p[0]] += 1;
        }

        vector<int> finished;

        auto rootList = getRootList(counts);
        while (!rootList.empty()) {
            finished.insert(end(finished), begin(rootList), end(rootList));
            
            for (auto root : rootList) {
                auto it = counts.find(root);
                if (it != end(counts)) {
                    counts.erase(it);
                }
                auto er = edges.equal_range(root);
                for (auto ti = er.first; ti != er.second; ++ti) {
                    auto vertex = ti->second;
                    it = counts.find(vertex);
                    it->second -= 1;
                }
            }

            rootList = getRootList(counts);
        }

        if (finished.size() != numCourses) {
            return {};
        }
        return finished;
    }

    vector<int> getRootList(const unordered_map<int, int>& counts) {
        vector<int> rv;
        for (const auto & p : counts) {
            if (p.second == 0) {
                rv.push_back(p.first);
            }
        }
        return rv;
    }
};
