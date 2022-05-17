#include <iostream>
#include <fstream>
#include <string>
#include <sstream> 
#include <vector>
//#include <set>
//#include <chrono>
//#include <algorithm>
#include <functional>
//#include <unistd.h>


bool dens(float d) {
    return rand()%100 < d; 
}

void write_edge_list(std::vector<std::tuple<uint64_t, uint64_t, double> > edges, uint64_t e, std::string filename){
    std::ofstream outfile(filename+".e");
    for(uint64_t j = 0; j < e; j++)
        outfile << std::get<0>(edges[j]) << " " << std::get<1>(edges[j]) << " " << std::get<2>(edges[j]) << std::endl;
    outfile << std::endl;
}

void write_node_set(uint64_t v, std::string filename){
    std::ofstream outfile(filename+".v");
    for(uint64_t j = 0; j < v; j++)
        outfile << j << std::endl;
    outfile << std::endl;
}

int main(int argc, char **argv) {
    // argv[1] -> number of nodes
    // argv[2] -> graph density (%)
    // argv[3] -> seed for PRNG
    // argv[4] -> -U (if undirected graph, default: directed graph)

    uint64_t num_nodes = std::stoi(argv[1]);
    float density = std::stof(argv[2]);
    int seed = std::stoi(argv[3]);
    srand(seed);

    // default: directed graph
    bool undirected = false;
    if (argc > 4){
        undirected = (std::string(argv[4]) == "-U") ? true : false; 
    }

    std::vector< std::tuple<uint64_t, uint64_t, double> > edges;
    std::tuple<uint64_t, uint64_t, double> edge_tmp;

    for(uint64_t i = 0; i < num_nodes; i++){
        for(uint64_t j = 0; j < num_nodes; j++){
            if(dens(density)){
                edge_tmp = std::make_tuple(i, j, (double)(rand()%100)/100.0);
                edges.push_back(edge_tmp);
            }
        } 
    }

    std::cout << "Num of edges = " << edges.size() << std::endl;
    std::stringstream filename;
    filename << "../data/N" << num_nodes << "_D" << density << "_S" << seed;

    write_edge_list(edges, edges.size(), filename.str());
    write_node_set(num_nodes, filename.str());

    return 0;
}
