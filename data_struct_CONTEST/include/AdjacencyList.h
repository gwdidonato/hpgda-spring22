#ifndef ORACLE_CONTEST_ADJACENCYLIST_H
#define ORACLE_CONTEST_ADJACENCYLIST_H

#include <list>
#include <vector>
#include <functional>

// Adjacency list implementation of Graph
class AdjacencyList{

    class EdgeIter {
        
        class iterator {
        public:
            iterator(std::list<uint64_t>::iterator ptr, std::list<double>::iterator begin_w_ptr) : ptr(ptr), begin_w_ptr(begin_w_ptr) {}

            iterator operator++() {
                ++ptr;
                ++begin_w_ptr;
                return *this;
            }

            bool operator!=(const iterator &other) { return ptr != other.ptr; }

            const std::pair<uint64_t, double> &operator*() {
                current.first = *ptr;
                current.second = *begin_w_ptr;
                return current;
            };

        private:
            std::list<uint64_t>::iterator ptr;
            std::list<double>::iterator begin_w_ptr;
            std::pair<uint64_t, double> current;
        };

    private:
        std::list<uint64_t>::iterator begin_ptr, end_ptr;
        std::list<double>::iterator begin_w_ptr;
    public:
        EdgeIter(std::list<uint64_t>::iterator begin_ptr, std::list<uint64_t>::iterator end_ptr, std::list<double>::iterator begin_w_ptr) : begin_ptr(begin_ptr), end_ptr(end_ptr), begin_w_ptr(begin_w_ptr) {}

        iterator begin() const { return iterator(begin_ptr, begin_w_ptr); }

        iterator end() const { return iterator(end_ptr, begin_w_ptr); }
    };

    uint64_t v, e;
    std::list<uint64_t>* edges;
    std::list<double>* weights;

public:

    EdgeIter get_neighbors(int idx){
        return EdgeIter(begin(idx), end(idx), begin_weights(idx));
    }

    AdjacencyList(uint64_t v, uint64_t e) : v(v), e(e){
        edges = new std::list<uint64_t>[v + 2];
        weights = new std::list<double>[v + 2];
        for(int i = 0; i <= v; ++i)
            edges[i].clear(), weights[i].clear();
    }

    ~AdjacencyList(){
        for(int i = 0; i <= v; ++i)
            edges[i].clear(), weights[i].clear();
        delete[] edges;
        delete[] weights;
        //std::cout<<"AdjacencyList delete"<<std::endl;
    }

    void add_edges(int from, std::vector<uint64_t>& to, std::vector<double>& w);

    void add_edge(int from, uint64_t to, double weight = 0);

    void sortEdgesByNodeId();

    void finished();

    void populate(std::tuple<uint64_t, uint64_t, double>* e_list);

    inline std::list<uint64_t>::iterator begin(int cur_vertex) {
        return edges[cur_vertex].begin();
    }

    inline std::list<uint64_t>::iterator end(int cur_vertex){
        return edges[cur_vertex].end();
    }

    inline std::list<double>::iterator begin_weights(int cur_vertex) {
        return weights[cur_vertex].begin();
    }
    
};


#endif //ORACLE_CONTEST_ADJACENCYLIST_H
