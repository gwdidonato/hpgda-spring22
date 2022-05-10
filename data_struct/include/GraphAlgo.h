#ifndef ORACLE_CONTEST_GRAPHALGO_HPP
#define ORACLE_CONTEST_GRAPHALGO_HPP

#include <cstring>
#include <queue>
#include <climits>
#include <set>

#define VERIFY_ENABLED false

template<typename T>
class GraphAlgo {
    uint64_t v, e;
    uint64_t *dist;
    bool *used;
    T *graph;

public:
    GraphAlgo(uint64_t v, uint64_t e) : v(v), e(e) {
        used = new bool[v + 2];
        dist = new uint64_t[v + 2];
        graph = new T(v, e);
    }

    ~GraphAlgo() {
        delete[] dist;
        delete[] used;
        delete graph;
    }

    inline void sortByEdgesByNodeId() {
        graph->sortByEdgesByNodeId();
    }

    // uint64_t bfs(uint32_t cur_vertex) {
    //     uint64_t sum = 0;
    //     bool* temp_used = used;
    //     memset(used, 0, sizeof(bool) * (v + 2));
    //     std::queue<uint32_t > q;
    //     q.push(cur_vertex);
    //     temp_used[cur_vertex] = true;
    //     auto lambdaFunction = [&sum, &q, &temp_used] (uint32_t edge, uint32_t weight){
    //         if (!temp_used[edge]) {
    //             sum = sum + weight;
    //             temp_used[edge] = true;
    //             q.push(edge);
    //         }
    //     };

    //     while (!q.empty()) {
    //         cur_vertex = q.front();
    //         q.pop();
    //         graph->applyAllEdges(cur_vertex, lambdaFunction);
    //     }
        
    //     return sum;

    // }

    std::vector<uint32_t> bfs_prev(uint32_t cur_vertex) {
        std::vector<uint32_t> result;
        memset(used, 0, sizeof(bool) * (v + 2));
        std::queue<uint32_t > q;
        q.push(cur_vertex);
        used[cur_vertex] = true;
        while (!q.empty()) {
            cur_vertex = q.front();
            q.pop();

            for (auto& to : graph->get_neighbors(cur_vertex)) {
                if (!used[to.first]) {
                    used[to.first] = true, q.push(to.first);
                }
            }
        }
        return sum;

    }

    float dfs_recursion(int cur_vertex) {
        float sum = 0;
        used[cur_vertex] = true;
        for (auto& to : graph->get_neighbors(cur_vertex)) {
            if (!used[to.first]) {
                sum += dfs_recursion(to.first), sum = sum + to.second;
            }
        }
        return sum;
    }

    float dfs(int cur_vertex) {
        memset(used, 0, sizeof(bool) * (v + 2));
        return dfs_recursion(cur_vertex);
    }

    void add_edge(int v, std::vector<uint32_t > &to, std::vector<float> &weights) {
        graph->add_edge(v, to, weights);
    }

    void add_edge(uint32_t from, uint32_t to, float weight) {
        graph->add_edge(from, to, weight);
    }

    void finished() {
        graph->finished();
    }

    // uint64_t dijkstra(int from, int to) {
    //     for (int i = 0; i < v + 2; ++i)
    //         dist[i] = LONG_LONG_MAX;
    //     dist[from] = 0;

    //     std::set<std::pair<long long, int>> min_heap;
    //     min_heap.insert(std::make_pair(0, from));

    //     while (!min_heap.empty()) {
    //         std::pair<uint64_t , int> temp = *(min_heap.begin());
    //         min_heap.erase(min_heap.begin());
    //         uint64_t distance = temp.first;
    //         int cur_vertex = temp.second;
    //         for (auto& to_pair : graph->get_neighbors(cur_vertex)) {
    //             uint32_t to_vertex = to_pair.first;
    //             uint32_t cur_weight = to_pair.second;
    //             if (distance + cur_weight < dist[to_vertex]) {
    //                 auto it_find = min_heap.find(std::make_pair(dist[to_vertex], to_vertex));
    //                 if (it_find != min_heap.end())
    //                     min_heap.erase(it_find);
    //                 dist[to_vertex] = distance + cur_weight;
    //                 min_heap.insert(std::make_pair(dist[to_vertex], to_vertex));
    //             }
    //         }
    //     }

    //     return dist[to];
    // }
};

#endif //ORACLE_CONTEST_GRAPHALGO_HPP
