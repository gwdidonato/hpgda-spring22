#include "../include/utils.h"
#include "../include/AdjacencyList.h"
#include "../include/GraphAlgo.h"
#include <fstream>
#include <ostream>
#include <string>


int main(int argc, char **argv) {
    // argv[1] -> graph name
    // argv[2] -> source vertex for BFS and DFS (default: 1)
    // argv[3] -> -U (if undirected graph, default: directed graph)

    // vertex stored in graphName.v
    // edges stored in graphName.e
    std::string graphName = argv[1];
    
    // default: directed graph
    bool undirected = false;
    if (argc > 3){
        undirected = (std::string(argv[3]) == "-U") ? true : false; 
    }

    // get number of edges
    uint64_t e = count_lines(graphName + ".e");
    if (undirected) e *= 2;

    // temporary data structs for nodes and edges
    std::tuple<uint64_t, uint64_t, double>* edges = new std::tuple<uint64_t, uint64_t, double>[e];
    std::set<uint64_t> nodes;

    // read nodes and edges
    std::cout << "Loading the graph " << graphName << std::endl;
    nodes = load_graph(graphName, undirected, edges, e);
    std::cout << "Graph loaded!" << std::endl << std::endl;

    // get number of nodes (TODO, get nodes from .v file instead)
    uint64_t v = nodes.size();
    
    // print graph info
    print_graph_info(v, e, undirected);

    // print edges
    // print_edges(edges, e);

    // instantiate the graph
    auto *graph = new GraphAlgo<AdjacencyList>(v,e);
    
    // populate the graph and measure time
    auto begin_populate = std::chrono::high_resolution_clock::now();
    graph->populate(edges);
    auto end_populate = std::chrono::high_resolution_clock::now();
    auto elapsed_populate = std::chrono::duration_cast<std::chrono::milliseconds>(end_populate - begin_populate);
    std::cout << "Graph population time: " << elapsed_populate.count() << " ms" << std::endl << std::endl;

    uint64_t src_vertex = 1;
    if (argc > 2){
        src_vertex = std::stoi(std::string(argv[2]));
    }
    double result = -1;
    
    // execute bfs and measure time
    auto begin_bfs = std::chrono::high_resolution_clock::now();
    result = graph->bfs(src_vertex);
    auto end_bfs = std::chrono::high_resolution_clock::now();
    auto elapsed_bfs = std::chrono::duration_cast<std::chrono::milliseconds>(end_bfs - begin_bfs);
    std::cout << "BFS execution time: " << elapsed_bfs.count() << " ms" << std::endl;
    std::cout << "BFS sum: " << result << std::endl;
    std::cout << "Writing BFS results..." << std::endl;
    graph->write_results(graphName + ".bfs");
    std::cout << "BFS results written in " << graphName + ".bfs" << std::endl << std::endl;

    // execute dfs and measure time
    auto begin_dfs = std::chrono::high_resolution_clock::now();
    result = graph->dfs(src_vertex);
    auto end_dfs = std::chrono::high_resolution_clock::now();
    auto elapsed_dfs = std::chrono::duration_cast<std::chrono::milliseconds>(end_dfs - begin_dfs);
    std::cout << "DFS execution time: " << elapsed_dfs.count() << " ms" << std::endl;
    std::cout << "DFS sum: " << result << std::endl;
    std::cout << "Writing DFS results..." << std::endl;
    graph->write_results(graphName + ".dfs");
    std::cout << "DFS results written in " << graphName + ".dfs" << std::endl << std::endl;
    delete graph;

    return 0;
}

