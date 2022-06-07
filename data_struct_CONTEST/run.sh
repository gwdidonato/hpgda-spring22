#!/bin/bash

# Run 10 iterations on example_directed, with src vertex 2
# Save results in results_directed.csv
echo "Benchmarking example_directed graph (10 iterations)"
echo "Src_Vertex,PopulateTime(ms),MemUsage(MB),BFSTime(ms),BFSSum,DFSTime(ms),DFSSum" > results_directed.csv
bin/exe data/example_directed 2 10 >> results_directed.csv 

# Run 10 iterations on example_undirected, with src vertex 2
# Save results in results_undirected.csv
echo "Benchmarking example_undirected graph (10 iterations)"
echo "Src_Vertex,PopulateTime(ms),MemUsage(MB),BFSTime(ms),BFSSum,DFSTime(ms),DFSSum" > results_undirected.csv
bin/exe data/example_undirected 2 10 -U >> results_undirected.csv 

# Run 10 iterations on wiki-Talk, with src vertex 2
# Save results in results_wiki-Talk.csv 
echo "Benchmarking wiki-Talk graph (10 iterations)"
echo "Src_Vertex,PopulateTime(ms),MemUsage(MB),BFSTime(ms),BFSSum,DFSTime(ms),DFSSum" > results_wiki-Talk.csv 
bin/exe eval_graphs/wiki-Talk/wiki-Talk 2 10 >> results_wiki-Talk.csv 
