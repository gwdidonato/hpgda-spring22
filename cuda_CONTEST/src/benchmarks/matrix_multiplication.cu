// Copyright (c) 2020, 2021, NECSTLab, Politecnico di Milano. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NECSTLab nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//  * Neither the name of Politecnico di Milano nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <sstream>
#include "matrix_multiplication.cuh"

namespace chrono = std::chrono;
using clock_type = chrono::high_resolution_clock;

//////////////////////////////
//////////////////////////////

// Use a 2D grid: each thread computes a value of the output matrix,
// i.e. the dot-product between x[i, :] and y[:, j];
__global__ void gpu_matrix_multiplication_0(double* x, double* y, double* z, int N) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;

    if (i < N && j < N) {
        double sum = 0;
        for (int k = 0; k < N; k++) {                
            sum += x[i * N + k] * y[k * N + j];
        }
        z[i * N + j] = sum;
    }
}

// Same as the first implementation, but with grid-stride loops;
__global__ void gpu_matrix_multiplication_1(double* x, double* y, double* z, int N) {
    for(int i = blockIdx.x * blockDim.x + threadIdx.x; i < N; i += blockDim.x * gridDim.x) {
        for(int j = blockIdx.y * blockDim.y + threadIdx.y; j < N; j += blockDim.y * gridDim.y) {
            double sum = 0;
            for (int k = 0; k < N; k++) {                
                sum += x[i * N + k] * y[k * N + j];
            }
            z[i * N + j] = sum;
        }
    }
}

// #define BLOCK_DIM 16
// __global__ void gpu_matrix_multiplication_2(double* x, double* y, double* z, int N) {

//     int tile_size = BLOCK_DIM;
//     int z_block_i = blockIdx.x;
//     int z_block_j = blockIdx.y;
//     // Coordinate of the Z matrix element computed by this specific thread, with respect to the current tile;
//     int z_i = threadIdx.x;
//     int z_j = threadIdx.y;
//     // Coordinate of the Z matrix element computed by this specific thread, with respect to the overall Z matrix (not counting host-level data partitioning);
//     int i = z_block_i * blockDim.x + threadIdx.x;
//     int j = z_block_j * blockDim.y + threadIdx.y;

//     // Value of the Z matrix block being computed by this specific thread;
//     double z_val_ij = 0;

//     // Loop over the tiles in the same row (for X) and column (for Y) of the desired output tile in Z;
//     for (int curr_block_index = 0; curr_block_index < N / tile_size; curr_block_index++) {
//         // Shared memory used to store the current tiles of X and Y;
//         __shared__ double x_tile[BLOCK_DIM][BLOCK_DIM];
//         __shared__ double y_tile[BLOCK_DIM][BLOCK_DIM];
//         x_tile[z_i][z_j] = x[N * i + curr_block_index * tile_size + z_j];
//         y_tile[z_i][z_j] = y[N * (z_i + curr_block_index * tile_size) + j];
//         __syncthreads();
//         // Multiply the i row and j column of the tile;
//         for (int k = 0; k < tile_size; k++) {   
//             z_val_ij += x_tile[z_i][k] * y_tile[k][z_j];
//         }
//         __syncthreads();
//     }
//     z[i * N + j] = z_val_ij;
// }

// Better implementation, using shared memory to compute square tiles of z;
__global__ void gpu_matrix_multiplication_2(double* x, double* y, double* z, int N) {

    // int tile_size = BLOCK_DIM;
    int tile_size = blockDim.x;

    // In the simplest implementation, each block computes a tile of the Z matrix, 
    // whose coordinates are given by blockIdx.x and blockIdx.y;
    // Here, we allow each block to process more tiles, hence the loops below;
    for(int z_block_i = blockIdx.x; z_block_i < (N + tile_size - 1) / tile_size; z_block_i += gridDim.x) {
        for(int z_block_j = blockIdx.y; z_block_j < (N + tile_size - 1) / tile_size; z_block_j += gridDim.y) {
            // Coordinate of the Z matrix element computed by this specific thread, with respect to the current tile;
            int z_i = threadIdx.x;
            int z_j = threadIdx.y;
            // Coordinate of the Z matrix element computed by this specific thread, with respect to the overall Z matrix (not counting host-level data partitioning);
            int i = z_block_i * blockDim.x + threadIdx.x;
            int j = z_block_j * blockDim.y + threadIdx.y;

            // Value of the Z matrix block being computed by this specific thread;
            double z_val_ij = 0;

            // Loop over the tiles in the same row (for X) and column (for Y) of the desired output tile in Z;
            for (int curr_block_index = 0; curr_block_index < (N + tile_size - 1) / tile_size; curr_block_index++) {
                // Shared memory used to store the current tiles of X and Y;
                extern __shared__ double tiles[];
                double *x_tile = tiles;
                double *y_tile = tiles + tile_size * tile_size;
                // Each thread in the block loads a value into the tile;
                if ((i < N) && (curr_block_index * tile_size + z_j < N)) {
                    x_tile[z_i * tile_size + z_j] = x[N * i + curr_block_index * tile_size + z_j];
                } else {
                    x_tile[z_i * tile_size + z_j] = 0;
                }
                if ((j < N) && (curr_block_index * tile_size + z_i < N)) {
                    y_tile[z_i * tile_size + z_j] = y[N * (curr_block_index * tile_size + z_i) + j];
                } else {
                    y_tile[z_i * tile_size + z_j] = 0;
                }
                // Synchronize threads in the block, ensure the tile has been loaded;
                __syncthreads();

                // Multiply the i row and j column of the tile;
                for (int k = 0; k < tile_size; k++) {   
                    z_val_ij += x_tile[z_i * tile_size + k] * y_tile[k * tile_size + z_j];
                }

                // Synchronize threads in the block, ensure the computation has finished before loading the next tile;
                __syncthreads();
            }
            // Write the output value into Z, keeping into account the offset of the current block;
            if ((i < N) & (j < N)) {
                z[i * N + j] = z_val_ij;
            } 
        }
    }
}

//////////////////////////////
//////////////////////////////

// Allocate data on the CPU and GPU;
void MatrixMultiplication::alloc() {
    // All square N x N matrices, with data stored as row-major;

    // Compute the number of blocks for implementations where the value is a function of the input size.
    // Note that in this case the grid is composed of (B x B) blocks, each with size (block_size x block_size);
    B = (N + block_size - 1) / block_size;
    
    // Allocate CPU data;
    x = (double*) malloc(sizeof(double) * N * N);
    y = (double*) malloc(sizeof(double) * N * N);
    z = (double*) malloc(sizeof(double) * N * N);
   
    // Allocate GPU data;
    err = cudaMalloc(&x_d, sizeof(double) * N * N);
    err = cudaMalloc(&y_d, sizeof(double) * N * N);
    err = cudaMalloc(&z_d, sizeof(double) * N * N);
}

// Initialize data;
void MatrixMultiplication::init() {
    // X and Y contains the same data
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            x[i * N + j] = double(i * N + j) / (N * N);
            y[i * N + j] = double(i * N + j) / (N * N);
        }
    }
}

// Reset the state of the computation after every iteration.
// Reset the result, and transfer data to the GPU if necessary;
void MatrixMultiplication::reset() {
    // Reset the result;
    memset(z, 0, sizeof(double) * N * N);
    // Reset result on the GPU;
    cudaMemset(z_d, 0, sizeof(double) * N * N);
    // Transfer data to the GPU;
    cudaMemcpy(x_d, x, sizeof(double) * N * N, cudaMemcpyHostToDevice);
    cudaMemcpy(y_d, y, sizeof(double) * N * N, cudaMemcpyHostToDevice);
}

void MatrixMultiplication::matrix_multiplication_0(int iter) {
    auto start_tmp = clock_type::now();
    // Call the GPU computation. In this case, the number of blocks depends on N;
    dim3 block_num_2d(B, B);
    dim3 block_size_2d(block_size, block_size);
    gpu_matrix_multiplication_0<<<block_num_2d, block_size_2d>>>(x_d, y_d, z_d, N);

    // Print performance of GPU, not accounting for transfer time;
    if (debug) {
        // Synchronize computation by hand to measure GPU exec. time;
        cudaDeviceSynchronize();
        auto end_tmp = clock_type::now();
        auto exec_time = chrono::duration_cast<chrono::microseconds>(end_tmp - start_tmp).count();
        std::cout << "  pure GPU execution(" << iter << ")=" << double(exec_time) / 1000 << " ms, " << (3 * sizeof(double) * N * N / (exec_time * 1e3)) << " GB/s" << std::endl;
    }

    // Copy the partial result from the GPU to the CPU;
    cudaMemcpy(z, z_d, sizeof(double) * N * N, cudaMemcpyDeviceToHost);
}

void MatrixMultiplication::matrix_multiplication_1(int iter) {
    auto start_tmp = clock_type::now();
    // Call the GPU computation. In this case, the number of blocks is chosen by the user;
    dim3 block_num_2d(num_blocks, num_blocks);
    dim3 block_size_2d(block_size, block_size);
    gpu_matrix_multiplication_1<<<block_num_2d, block_size_2d>>>(x_d, y_d, z_d, N);

    // Print performance of GPU, not accounting for transfer time;
    if (debug) {
        // Synchronize computation by hand to measure GPU exec. time;
        cudaDeviceSynchronize();
        auto end_tmp = clock_type::now();
        auto exec_time = chrono::duration_cast<chrono::microseconds>(end_tmp - start_tmp).count();
        std::cout << "  pure GPU execution(" << iter << ")=" << double(exec_time) / 1000 << " ms, " << (3 * sizeof(double) * N * N / (exec_time * 1e3)) << " GB/s" << std::endl;
    }

    // Copy the partial result from the GPU to the CPU;
    cudaMemcpy(z, z_d, sizeof(double) * N * N, cudaMemcpyDeviceToHost);
}

void MatrixMultiplication::matrix_multiplication_2(int iter) {
    auto start_tmp = clock_type::now();
    // Call the GPU computation. In this case, the number of blocks is chosen by the user;
    dim3 block_num_2d(num_blocks, num_blocks);
    dim3 block_size_2d(block_size, block_size);
    gpu_matrix_multiplication_2<<<block_num_2d, block_size_2d, 2 * sizeof(double) * block_size * block_size>>>(x_d, y_d, z_d, N);

    // Print performance of GPU, not accounting for transfer time;
    if (debug) {
        // Synchronize computation by hand to measure GPU exec. time;
        cudaDeviceSynchronize();
        auto end_tmp = clock_type::now();
        auto exec_time = chrono::duration_cast<chrono::microseconds>(end_tmp - start_tmp).count();
        std::cout << "  pure GPU execution(" << iter << ")=" << double(exec_time) / 1000 << " ms, " << (3 * sizeof(double) * N * N / (exec_time * 1e3)) << " GB/s" << std::endl;
    }

    // Copy the partial result from the GPU to the CPU;
    cudaMemcpy(z, z_d, sizeof(double) * N * N, cudaMemcpyDeviceToHost);
}

// In this example, use CUBLAS;
void MatrixMultiplication::matrix_multiplication_3(int iter) {
    auto start_tmp = clock_type::now();
    double alpha = 1;
    double beta = 0;
    // You can pass CUBLAS_OP_T (instead of CUBLAS_OP_N) to say that matrices should be transposed;
    cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, N, N, N, &alpha, x_d, N, y_d, N, &beta, z_d, N);
    // Print performance of GPU, not accounting for transfer time;
    if (debug) {
        // Synchronize computation by hand to measure GPU exec. time;
        cudaDeviceSynchronize();
        auto end_tmp = clock_type::now();
        auto exec_time = chrono::duration_cast<chrono::microseconds>(end_tmp - start_tmp).count();
        std::cout << "  pure GPU execution(" << iter << ")=" << double(exec_time) / 1000 << " ms, " << (3 * sizeof(double) * N * N / (exec_time * 1e3)) << " GB/s" << std::endl;
    }

    // Copy the partial result from the GPU to the CPU;
    cudaMemcpy(z, z_d, sizeof(double) * N * N, cudaMemcpyDeviceToHost);
}

void MatrixMultiplication::execute(int iter) {
    switch (implementation)
    {
    case 0:
        matrix_multiplication_0(iter);
        break;
    case 1:
        matrix_multiplication_1(iter);
        break;
    case 2:
        matrix_multiplication_2(iter);
        break;
    case 3:
        matrix_multiplication_3(iter);
        break;
    default:
        break;
    }
}

// Compute the sum of values of CPU and GPU results, and use that as checksum.
// Also count the number of different CPU and GPU results.
// The CPU implementation does not really perform a standard matrix multiplication,
// but the memory access pattern is identical to a standard matrix multiplication,
// and execution time should be similar as well;
void MatrixMultiplication::cpu_validation(int iter) {
    auto start_tmp = clock_type::now();
    unsigned int errors = 0;
    double sum_total = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0;
            for (int k = 0; k < N; k++) {
                sum += x[i * N + k] * y[k * N + j];
            }
            double z_res = z[i * N + j];
            sum_total += sum / (N * N);
            gpu_checksum += z_res / (N * N);
            if (std::abs(z_res - sum) > 1e-4) {
                errors += 1;
            }
        }
    }
    auto end_tmp = clock_type::now();
    auto exec_time = chrono::duration_cast<chrono::microseconds>(end_tmp - start_tmp).count();
    std::cout << "exec time CPU=" << double(exec_time) / 1000 << " ms" << std::endl;
    if (errors > 0) std::cout << "result error! errors=" << errors << "; GPU checksum=" << gpu_checksum << ", CPU checksum=" << sum_total << std::endl; 
}

std::string MatrixMultiplication::print_result(bool short_form) {
    if (short_form) {
        return std::to_string(z[0]);
    } else {
        std::ostringstream out;
        out.precision(3);
        out << "[\n";
        for (int i = 0; i < std::min(5, N); i++) {
            out << "[";
            for (int j = 0; j < std::min(5, N); j++) {
                out << z[i * N + j] << ", ";
            }
            out << "...]\n";
        }
        out << "...]";
        return out.str();
    }
}

void MatrixMultiplication::clean() {
    free(x);
    free(y);
    free(z);
    cudaFree(x_d);
    cudaFree(y_d);
    cudaFree(z_d);
    cublasDestroy(handle);
}
