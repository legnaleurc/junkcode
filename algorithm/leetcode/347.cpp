class Solution {
public:
    vector<int> topKFrequent(vector<int>& nums, int k) {
        unordered_map<int, int> freq;
        for (auto n : nums) {
            auto it = freq.find(n);
            if (it == end(freq)) {
                freq.insert({ n, 1 });
            } else {
                ++it->second;
            }
        }

        using Pair = pair<int, int>;
        auto cmp = [](const Pair & l, const Pair & r) -> bool {
            return l.second > r.second;
        };
        priority_queue<Pair, vector<Pair>, decltype(cmp)> q{cmp};
        for (const auto & p : freq) {
            if (q.size() < k) {
                q.push(p);
                continue;
            }
            q.push(p);
            q.pop();
        }

        vector<int> rv;
        while (!q.empty()) {
            rv.push_back(q.top().first);
            q.pop();
        }
        reverse(begin(rv), end(rv));
        return rv;
    }
};
