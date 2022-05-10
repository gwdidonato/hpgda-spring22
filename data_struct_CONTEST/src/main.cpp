#include <iostream>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <functional>
#include <ostream>
#include <string>
#include <sstream> 
#include <set>
#include "../include/ALGraph.h"
#include "../include/GraphAlgo.h"

unsigned int count_lines(std::string filename){
    std::ifstream inFile(filename);
    unsigned int numLines = 0;
    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty())
            numLines++;
    }
    return numLines;
}

std::vector<std::string> split (const std::string &s, char delim){
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

int main(int argc, char **argv) {
    // argv[1] -> graph name
    // argv[2] -> -U (if undirected graph, default: directed graph)

    // vertex stored in graphName.v
    // edges stored in graphName.e
    std::string graphName = argv[1];
    
    // default: directed graph
    bool undirected = false;
    if (argc > 2){
        undirected = (std::string(argv[2]) == "-U") ? true : false; 
    }

    unsigned int e = count_lines(graphName + ".e");
    if (undirected) e *= 2;

    std::tuple<uint32_t, uint32_t, float>* edges = new std::tuple<uint32_t, uint32_t, float>[e];
    std::set<uint32_t> nodes;
    
    std::ifstream eFile(graphName+".e");
    std::string line;
    std::vector<std::string> tmp;
    unsigned int i = 0;
    while (std::getline(eFile, line)){
        if (!line.empty()){
            tmp = split(line, ' ');
            edges[i] = std::make_tuple(std::stoi(tmp[0]), std::stoi(tmp[1]), std::stof(tmp[2]));
            if (undirected) 
                edges[e/2+i] = std::make_tuple(std::stoi(tmp[1]), std::stoi(tmp[0]), std::stof(tmp[2]));
            nodes.insert(std::stoi(tmp[0]));
            nodes.insert(std::stoi(tmp[1]));
            i++;
        }
    }

    unsigned int v = nodes.size();
    
    std::string prop = (undirected) ? "Undirected" : "Directed";
    std::cout << prop << " graph" << std::endl;
    std::cout << "num nodes: " << v << std::endl;
    std::cout << "num directed edges: " << e << std::endl;

    // for(int j = 0; j < e; j++){
    //     std::cout << std::get<0>(edges[j]) << " " << std::get<1>(edges[j]) << " " << std::get<2>(edges[j]) << std::endl;
    // }

    auto *graph = new GraphAlgo<ALGraph>(v,e);
    for(i = 0; i < e; i++){
        graph->add_edge(std::get<0>(edges[i]), std::get<1>(edges[i]), std::get<2>(edges[i]));
        proof += std::get<2>(edges[i]);
    }
    graph->finished();
    float result = graph->bfs_prev(1);

    return 0;
}

