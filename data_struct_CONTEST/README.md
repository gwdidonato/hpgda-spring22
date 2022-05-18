# Data Structure Contest - Spring 2022
In the Data Structure track of the contest you will have the chance to play with different **sparse graph representations** and explore their runtime performance. 

## Description
The goal of this challenge consists in implementing a graph data structure to store a graph, 
and an algorithm to efficiently populate your data structure with real graph data.

## Dataset
You will work on datasets provided by the [Linked Data Benchmark Council (LDBC)](https://ldbcouncil.org/).
In particular, you will work on graphs from the [LDBC Graphanalytics Benchmark suite](https://ldbcouncil.org/benchmarks/graphalytics/).
Some example graphs are already available in the ```data``` subfolder.
The data format is described in the [LDBC Graphanalytics Benchmark Specification](https://arxiv.org/pdf/2011.15028.pdf).
The data loader is already provided in  ```include/utils.h```, and you can use it as it is, unless you want to make it faster (but it's just a side quest)!

## Challenge
The contest consists in implementing an graph data structure that is efficient both in terms of memory usage and runtime performance.
You can use every graph representation you want (e.g. AdjacencyList, CSR, COO), **except the Adjacency Matrix**.
Your implementation must interface with the ```GraphAlgorithm``` class, by exposing the following public methods:
* populate 
* get_neighbors
* finished
You can implement any other methods you may need.
You must update ```src/main.cpp``` to use your data structure during the execution.
You are not required to update the ```GraphAlgorithm``` class (if you do, tell us how you changed it and why).

## Evaluation
Your solution will be evaluated w.r.t. the following metrics:
* graph population time
* graph size in memory (that's why you don't want to use the Adjacency Matrix!)
* execution time of the BFS and DFS algorithms implemented in ```GraphAlgorithm```
The ```src/main.cpp``` code already measures all this values.

## Build and run the code
An example implementation is provided in ```AdjacencyList.h```  and ```AdjacencyList.cpp```. 
To build the example, just run ```make``` in this folder.
To run the example on the ```example-directed``` graph:
``` 
bin/exe data/example-directed
``` 

## Submission
You must submit your solution by June 26th 11.59 PM. 

The submission consists in an email to Guido Walter Di Donato (`guidowalter.didonato at polimi.it`) with CC Marco Santambrogio (`marco.santambrogio at polimi.it`),
containing the name of the partecipants, and a link to the `git` repository on which you developed your solution.

The repository must contain:
* The source code
* A report describing any implementative decision you took that you'd like to share with us, and your findings in terms of performance of the implemented data structure compared to the provided `AdjacencyList`.

Please note:

* Your code must be buildable with standard tools like Maven.
* Tests must be runnable using a bash or python script.
* Keep in mind: the easier for us to replicate your results, the better it is for you!
* External libraries are allowed, as long as you justify their usage and the *core* of the implementation is written by you. You can use existing implementations, but only as a performance comparison against your custom implementation.
* The report should be 6 pages long at most, and written in single- or double-column Latex, with font-size 10pt.

