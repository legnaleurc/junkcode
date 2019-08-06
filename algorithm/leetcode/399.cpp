class Graph {
    unordered_map<string, vector<pair<string, double>>> vertaxs;
public:
    Graph() {}

    void add(const string& b, const string& e, double v) {
        // multiply edge
        auto it = vertaxs.find(b);
        if (it == end(vertaxs)) {
            vertaxs.insert({ b, {{ e, v }} });
        } else {
            auto & edges = it->second;
            edges.push_back({ e, v });
        }
        // reverse (i.e. division) edge
        it = vertaxs.find(e);
        if (it == end(vertaxs)) {
            vertaxs.insert({ e, {{ b, 1.0 / v }} });
        } else {
            auto & edges = it->second;
            edges.push_back({ b, 1.0 / v });
        }
    }

    double search(const string& b, const string& e) {
        auto it = vertaxs.find(b);
        if (it == end(vertaxs)) {
            return -1.;
        }
        if (b == e) {
            return 1.;
        }
        it = vertaxs.find(e);
        if (it == end(vertaxs)) {
            return -1.;
        }
        set<string> visited;
        return search(b, e, visited);
    }
    
private:
    double search(const string& b, const string& e, set<string>& visited) {
        // found
        if (b == e) {
            visited.insert(b);
            return 1.0;
        }

        // visited, no answer
        auto vd = visited.find(b);
        if (vd != end(visited)) {
            return 0.0;
        }
        visited.insert(b);

        // preconditions ensures b always in the graph
        auto it = vertaxs.find(b);
        if (it == end(vertaxs)) {
            assert(!"no way");
        }
        // reached leaf in the graph, should not happen
        auto & edges = it->second;
        if (edges.empty()) {
            return 0.0;
        }

        // DFS the graph
        for (auto & edge : edges) {
            auto rv = search(edge.first, e, visited);
            if (rv > 0) {
                return edge.second * rv;
            }
        }

        // should not happen
        return -1.;
    }
};


class Solution {
public:
    vector<double> calcEquation(vector<vector<string>>& equations, vector<double>& values, vector<vector<string>>& queries) {
        Graph graph;
        for (int i = 0; i < equations.size(); ++i) {
            graph.add(equations[i][0], equations[i][1], values[i]);
        }
        vector<double> rv;
        for (auto & q : queries) {
            rv.push_back(graph.search(q[0], q[1]));
        }
        return rv;
    }
};
