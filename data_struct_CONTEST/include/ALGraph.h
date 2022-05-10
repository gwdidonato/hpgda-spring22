#ifndef ORACLE_CONTEST_ALGRAPH_H
#define ORACLE_CONTEST_ALGRAPH_H

#include <list>
#include <vector>
#include <functional>

// Adjacency list implementation of Graph
class ALGraph{

    class EdgeIter {
        
        class iterator {
        public:
            iterator(std::list<uint32_t>::iterator ptr, std::list<float>::iterator begin_w_ptr) : ptr(ptr), begin_w_ptr(begin_w_ptr) {}

            iterator operator++() {
                ++ptr;
                ++begin_w_ptr;
                return *this;
            }

            bool operator!=(const iterator &other) { return ptr != other.ptr; }

            const std::pair<uint32_t, float> &operator*() {
                current.first = *ptr;
                current.second = *begin_w_ptr;
                return current;
            };

        private:
            std::list<uint32_t>::iterator ptr;
            std::list<float>::iterator begin_w_ptr;
            std::pair<uint32_t, float > current;
        };

    private:
        std::list<uint32_t>::iterator begin_ptr, end_ptr;
        std::list<float>::iterator begin_w_ptr;
    public:
        EdgeIter(std::list<uint32_t>::iterator begin_ptr, std::list<uint32_t>::iterator end_ptr, std::list<float>::iterator begin_w_ptr) : begin_ptr(begin_ptr), end_ptr(end_ptr), begin_w_ptr(begin_w_ptr) {}

        iterator begin() const { return iterator(begin_ptr, begin_w_ptr); }

        iterator end() const { return iterator(end_ptr, begin_w_ptr); }
    };

    uint64_t v, e;
    std::list<uint32_t>* edges;
    std::list<float>* weights;

public:

    EdgeIter get_neighbors(int idx){
        return EdgeIter(begin(idx), end(idx), begin_weights(idx));
        //return edges[idx];
    }

    ALGraph(uint64_t v, uint64_t e) : v(v), e(e){
        edges = new std::list<uint32_t>[v + 2];
        weights = new std::list<float>[v + 2];
        for(int i = 0; i <= v; ++i)
            edges[i].clear(), weights[i].clear();
    }

    ~ALGraph(){
        for(int i = 0; i <= v; ++i)
            edges[i].clear(), weights[i].clear();
        delete[] edges;
        delete[] weights;
        //std::cout<<"ALGraph delete"<<std::endl;
    }

    void add_edge(int from, std::vector<uint32_t>& to, std::vector<float>& w);

    void add_edge(int from, uint32_t to, float weight = 0);

    void finished();

    void sortByEdgesByNodeId();

    template<typename CB>
    void applyAllEdges(uint32_t from, CB lambdaCallback){
        std::list<uint32_t>::iterator begin_edges, end_edges;
        std::list<float>::iterator begin_weights_ptr;
        begin_edges = begin(from);
        end_edges = end(from);
        begin_weights_ptr = begin_weights(from);

        for(; begin_edges != end_edges; ++begin_edges, ++begin_weights_ptr)
            lambdaCallback(*begin_edges, *begin_weights_ptr);
    };

    inline std::list<uint32_t>::iterator begin(int cur_vertex) {
        return edges[cur_vertex].begin();
    }

    inline std::list<uint32_t>::iterator end(int cur_vertex){
        return edges[cur_vertex].end();
    }

    inline std::list<float>::iterator begin_weights(int cur_vertex) {
        return weights[cur_vertex].begin();
    }
};


#endif //ORACLE_CONTEST_ALGRAPH_H
