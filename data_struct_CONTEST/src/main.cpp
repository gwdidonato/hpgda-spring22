#include <iostream>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <functional>
#include <ostream>
#include <string>
#include <sstream> 
#include <set>
#include "../include/utils.h"
#include "../include/ALGraph.h"
#include "../include/GraphAlgo.h"


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

    // get number of edges
    unsigned int e = count_lines(graphName + ".e");
    if (undirected) e *= 2;

    // temporary data structs for nodes and edges
    std::tuple<uint32_t, uint32_t, float>* edges = new std::tuple<uint32_t, uint32_t, float>[e];
    std::set<uint32_t> nodes;

    // read nodes and edges
    nodes = load_graph(graphName, undirected, edges, e);

    // get number of nodes (TODO, get nodes from .v file instead)
    unsigned int v = nodes.size();
    
    // print graph info
    // print_graph_info(v, e, undirected);

    // print edges
    // print_edges(edges, e);

    // instantiate the graph
    auto *graph = new GraphAlgo<ALGraph>(v,e);
    
    // populate the graph and measure time
    auto begin_populate = std::chrono::high_resolution_clock::now();
    graph->populate(edges);
    auto end_populate = std::chrono::high_resolution_clock::now();
    auto elapsed_populate = std::chrono::duration_cast<std::chrono::milliseconds>(end_populate - begin_populate);
    std::cout << "Graph population time: " << elapsed_populate.count() << " ms" << std::endl;

    float result = -1;
    
    // execute bfs and measure time
    auto begin_bfs = std::chrono::high_resolution_clock::now();
    result = graph->bfs(2);
    auto end_bfs = std::chrono::high_resolution_clock::now();
    auto elapsed_bfs = std::chrono::duration_cast<std::chrono::milliseconds>(end_bfs - begin_bfs);
    std::cout << "BFS sum: " << result << std::endl;
    std::cout << "BFS execution time: " << elapsed_bfs.count() << " ms" << std::endl;

    // execute dfs and measure time
    auto begin_dfs = std::chrono::high_resolution_clock::now();
    result = graph->dfs(2);
    auto end_dfs = std::chrono::high_resolution_clock::now();
    auto elapsed_dfs = std::chrono::duration_cast<std::chrono::milliseconds>(end_dfs - begin_dfs);
    std::cout << "DFS sum: " << result << std::endl;
    std::cout << "DFS execution time: " << elapsed_dfs.count() << " ms" << std::endl;

    

    return 0;
}

