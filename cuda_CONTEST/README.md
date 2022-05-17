# CUDA Examples
 
A small and extensible benchmark suite to implement and test algorithms in CUDA. 
Currently, two and a half algorithms are available:
1. `vector_sum`: it computes the sum of a numerical vector of size `N`, as presented [here](https://developer.download.nvidia.com/assets/cuda/files/reduction.pdf) and [here](https://developer.nvidia.com/blog/faster-parallel-reductions-kepler/).
2. `matrix_multiplication`: it computes the dense matrix product of two `N x N` matrices, as shown [here](https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#shared-memory).
3. `ppr`: Personalized PageRank, computed over an undirected graph for random personalization vertices. The GPU implementation is missing, it's your task to create it!

## Building the code

* `make`: it will create a `bin` folder and a `b` executable inside it.
* You need `nvcc` and `cuBLAS` to be available.
* Make sure to update the `Makefile` to match your GPU architecture (open the file for more information)
* This has been tested on Linux and on Windows 11 using [WSL2](https://docs.nvidia.com/cuda/wsl-user-guide/index.html)

## Run the code

```
bin/b -d -c -n 1000 -b vec -I 1 -i 30 -t 32 -B 64
```

**Warning:** If running on Kepler GPUs (E.g. Nvidia K80), change branch with `git checkout kepler` to run code compatible with this GPU. The only difference is that the VectorSym example uses single-precision floating-point arithmetic, as `atomicAdd` on double-precision was not available in this architecture. Be aware of numerical issues when using large inputs and single-precision arithmetic! 

**Options**
* `-n`: size of the benchmark, e.g. number of elements in a vector or number of rows/colums in a square matrix. **Unused for Personalized PageRank**
* `-i`: number of repetitions of the test (30 by default)
* `-b`: name of the benchmark (`vec`, `mmul`, `ppr`)
* `-d`: if true, print debug results. Else, print a CSV-like format. If you change the code, make sure that nothing but the CSV is printed when `-d` is not present.
* `-c`: if true, validate the correctness of the output. It might take a while on large inputs!
* `-t`: number of threads in a GPU block. If using 2D or 3D blocks, this is the size of the dimension (i.e. if `-t` is 8 and you use a 2D block, the block has 64 threads). The number of threads in a block cannot exceed 1024.
* `-B`: number of blocks in the grid, for implementations where this setting is available. If using 2D or 3D blocks, this is the size of the dimension.
* `-I`: implementation to use for a given benchmark. For example, `vec` has 3 different implementations (0, 1, 2).
* `-s`: warm-up iterations to skip when computing the average execution time (3 by default).
* `-v`: if using `nvprof`, using `-v` ensures that the profiler captures only the GPU computation and not the benchmark initialization or CPU validation, making profiling simpler. 

**Additional options for Personalized PageRank**
* `-g`: path to the MTX file of the input graph (`California` by default)
* `-a`: value of alpha (damping factor), from 0 to 1 (0.85 by default)
* `-m`: maximum number of iterations done in PPR (30 by default)
* `-e`: convergence threshold that must be met to stop the computation before the maximum number of iterations has been reached (`1e-6` by default)


## Contest: creating the Personalized PageRank benchmark

If you want to implement the Personalized PageRank benchmark, you need to implement the missing function in the PageRank class. 
The graph loading is already there for you, but you need to to the following:
1. `alloc`: allocate GPU memory or additional CPU memory.
2. `init`: initialize the PageRank computation (e.g. starting values of the PPR vector).
3. `reset`: reset the result and transfer data to the GPU.
4. `execute`: if you want to have multiple implementations, follow the pattern used in the other benchmarks. 
Otherwise, here you need to have the GPU host code responsible of starting and synchronizing the computation, and transferring the output from the GPU to the CPU.
5. `cpu_validation`: it computes the PPR computation on the CPU. GPU results must be moved to the `pr` array, so that they can be checked.
6. `print_result`: used for debugging, you can print the output in whatever format you prefer.
7. `clean`: deallocate GPU memory or additional CPU memory.

Things to keep in mind:
1. **Make sure that the code can still be compiled with `make`, and no extra steps.**
2. **Make sure that the code prints the standard CSV with the result, and nothing else, when not using the debug flag.**
3. **Create a run.sh script to execute the benchmark, with whatever input parameter (grid size, implementation etc.) you prefer** 
