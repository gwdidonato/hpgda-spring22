#ifndef ORACLE_CONTEST_GRAPHALGORITHM_H
#define ORACLE_CONTEST_GRAPHALGORITHM_H

#include <cstring>
#include <fstream>
#include <iostream>
#include <ostream>
#include <queue>
#include <climits>
#include <set>
#include <string>

template<typename T>
class GraphAlgorithm {
    uint64_t v, e, last;
    uint64_t *dist;
    bool *used;
    T *graph;

public:
    GraphAlgorithm(uint64_t v, uint64_t e) : v(v), e(e) {
        used = new bool[v + 2];
        dist = new uint64_t[v + 2];
        graph = new T(v, e);
    }

    ~GraphAlgorithm() {
        delete[] used;
        delete[] dist;
        delete graph;
    }

    // required
    void populate(std::tuple<uint64_t, uint64_t, double>* edges) {
        graph->populate(edges);
    }
    
    // required
    void finished() {
        graph->finished();
    }

    // optional
    void sortEdgesByNodeId() {
        graph->sortEdgesByNodeId();
    }

    // optional
    void add_edges(uint64_t from, std::vector<uint64_t> &to, std::vector<double> &weights) {
        graph->add_edges(from, to, weights);
    }

    // optional
    void add_edge(uint64_t from, uint64_t to, double weight) {
        graph->add_edge(from, to, weight);
    }

    void write_results(std::string filename) {
        std::ofstream outfile(filename);
        for (uint64_t i = 0; i <= v; i++){
            outfile << i << " " << dist[i] << std::endl; 
        }
    }

    // the bfs populate diff with the corresponding 
    // layer of the BFS tree for each vertex
    double bfs(uint64_t cur_vertex) {
        // initialization
        memset(used, 0, sizeof(bool) * (v + 2));
        for (uint64_t i = 0; i < v + 2; i++)
            dist[i] = LONG_MAX; 
        double sum = 0;
        std::queue<uint64_t> q;
        q.push(cur_vertex);
        used[cur_vertex] = true;
        dist[cur_vertex] = 0;

        // main loop
        while (!q.empty()) {
            cur_vertex = q.front();
            q.pop();
            for (auto& to : graph->get_neighbors(cur_vertex)) {
                if (!used[to.first]) {
                    used[to.first] = true;
                    dist[to.first] = dist[cur_vertex] + 1;
                    q.push(to.first);
                    sum = sum + to.second;
                }
            }
        }
        return sum;
    }

    // the dfs populate diff with the visiting 
    // order for each vertex
    double dfs_recursion(uint64_t cur_vertex) {
        double sum = 0;
        used[cur_vertex] = true;
        for (auto& to : graph->get_neighbors(cur_vertex)) {
            if (!used[to.first]) {
                dist[to.first] = ++last;
                sum += to.second;
                sum += dfs_recursion(to.first);
            }
        }
        return sum;
    }

    double dfs(uint64_t cur_vertex) {
        // initialization
        memset(used, 0, sizeof(bool) * (v + 2));
        for (uint64_t i = 0; i < v + 2; i++)
            dist[i] = LONG_MAX;
        dist[cur_vertex] = 0;
        last = 0;
        // recursion
        return dfs_recursion(cur_vertex);
    }

};

#endif //ORACLE_CONTEST_GRAPHALGORITHM_H
