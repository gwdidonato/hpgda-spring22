#include <iostream>
// #include "../include/ALGraph.h"
// #include "../include/GraphAlgo.h"
#include <chrono>
#include <algorithm>
#include <fstream>
#include <functional>
#include <ostream>
#include <string>

unsigned int count_lines(std::string filename){
    std::ifstream inFile(filename);
    unsigned int numLines = 0;
    std::string line;
    while(std::getline(inFile, line)){
        if(!line.empty())
            numLines++;
    }
    return numLines;
}


int main(int argc, char **argv) {
    // argv[1] -> graph name
    // argv[2] -> D (directed) or U (undirected)

    std::string graphName = argv[1];
    // std::ifstream vFile(graphName+".v");
    // std::ifstream eFile(graphName+".e");

    unsigned int v = count_lines(graphName+".v");
    unsigned int e = count_lines(graphName+".e");

    std::cout << "num nodes: " << v << std::endl;
    std::cout << "num edges: " << e << std::endl;
    
    return 0;
}

