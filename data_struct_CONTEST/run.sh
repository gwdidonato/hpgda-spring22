#!/bin/bash

# Run 10 iterations on example_directed, with src vertex 2
# Save results in results_directed.csv
echo "PopulateTime(ms),MemUsage(MB),BFSTime(ms),BFSSum,DFSTime(ms),DFSSum" > results_directed.csv
for i in {1..10}
do
    echo "Directed: Iteration $i"
    bin/exe data/example_directed 2 >> results_directed.csv 
done

# Run 10 iterations on example_undirected, with src vertex 2
# Save results in results_undirected.csv
echo "PopulateTime(ms),MemUsage(MB),BFSTime(ms),BFSSum,DFSTime(ms),DFSSum" > results_undirected.csv
for i in {1..10}
do
    echo "Undirected: Iteration $i"
    bin/exe data/example_undirected 2 -U >> results_undirected.csv 
done
