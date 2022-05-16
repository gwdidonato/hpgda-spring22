#ifndef ORACLE_CONTEST_UTILS_H
#define ORACLE_CONTEST_UTILS_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream> 
#include <vector>
#include <set>
#include <chrono>
#include <algorithm>
#include <functional>

// count non empty lines in a file
uint64_t count_lines(std::string filename){
    std::ifstream inFile(filename);
    uint64_t numLines = 0;
    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty())
            numLines++;
    }
    return numLines;
}

// split a string in a vector of strings, given a separator char
std::vector<std::string> split(const std::string &s, char delim){
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

// load the graph
std::set<uint64_t> load_graph(std::string filename, bool undirected, std::tuple<uint64_t, uint64_t, double>* edges, uint64_t e){
    std::ifstream eFile(filename+".e");
    std::string line;
    std::vector<std::string> tmp;    
    std::set<uint64_t> nodes;

    while (std::getline(eFile, line)){
        if (!line.empty()){
            tmp = split(line, ' ');
            break;
        }
    }
    eFile.seekg(0);

    bool weighted = false;
    if (tmp.size() == 3)
        weighted = true;

    uint64_t i = 0;

    // TODO make it faster
    while (std::getline(eFile, line)){
        if (!line.empty()){
            tmp = split(line, ' ');
            edges[i] = std::make_tuple(std::stoi(tmp[0]), std::stoi(tmp[1]), (weighted)?std::stof(tmp[2]):1);
            if (undirected) 
                edges[e/2+i] = std::make_tuple(std::stoi(tmp[1]), std::stoi(tmp[0]), (weighted)?std::stof(tmp[2]):1);
            nodes.insert(std::stoi(tmp[0]));
            nodes.insert(std::stoi(tmp[1]));
            i++;
        }
    }
    return nodes;
}

void print_graph_info(uint64_t v, uint64_t e, bool undirected){
    std::string prop = (undirected) ? "Undirected" : "Directed";
    std::cout << prop << " graph" << std::endl;
    std::cout << "num nodes: " << v << std::endl;
    std::cout << "num directed edges: " << e << std::endl << std::endl;
}

void print_edges(std::tuple<uint64_t, uint64_t, double>* edges, uint64_t e){
    for(uint64_t j = 0; j < e; j++)
        std::cout << std::get<0>(edges[j]) << " " << std::get<1>(edges[j]) << " " << std::get<2>(edges[j]) << std::endl;
    std::cout << std::endl;
}

#endif //ORACLE_CONTEST_UTILS_H
