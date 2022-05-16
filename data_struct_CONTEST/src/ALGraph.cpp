#include "../include/ALGraph.h"

void ALGraph::add_edge(int from, std::vector<uint32_t>& to, std::vector<float>& w) {
    for(uint32_t i : to)
        edges[from].push_back(i);
    for(float i : w)
        weights[from].push_back(i);
}

void ALGraph::finished() {
}

void ALGraph::add_edge(int from, uint32_t to, float weight) {
    edges[from].push_back(to);
    weights[from].push_back(weight);
}

void ALGraph::sortByEdgesByNodeId() {
    for(int i = 1; i <= v; ++i)
        edges[i].sort();
}

void ALGraph::populate(std::tuple<uint32_t, uint32_t, float>* e_list){
    for(unsigned int i = 0; i < e; i++)
        add_edge(std::get<0>(e_list[i]), std::get<1>(e_list[i]), std::get<2>(e_list[i]));
    finished();
}