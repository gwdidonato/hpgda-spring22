#include "../include/AdjacencyList.h"

void AdjacencyList::add_edges(int from, std::vector<uint64_t>& to, std::vector<double>& w) {
    for(uint64_t i : to)
        edges[from].push_back(i);
    for(double i : w)
        weights[from].push_back(i);
}

void AdjacencyList::add_edge(int from, uint64_t to, double weight) {
    edges[from].push_back(to);
    weights[from].push_back(weight);
}

void AdjacencyList::populate(std::tuple<uint64_t, uint64_t, double>* e_list){
    for(uint64_t i = 0; i < e; i++)
        add_edge(std::get<0>(e_list[i]), std::get<1>(e_list[i]), std::get<2>(e_list[i]));
    finished();
}

void AdjacencyList::sortByEdgesByNodeId() {
    for(uint64_t i = 0; i <= v; ++i)
        edges[i].sort();
}

void AdjacencyList::finished() {}