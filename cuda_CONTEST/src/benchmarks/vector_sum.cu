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

#include "vector_sum.cuh"

namespace chrono = std::chrono;
using clock_type = chrono::high_resolution_clock;

//////////////////////////////
//////////////////////////////

__global__ void gpu_vector_sum_0(double *x, double *res_tmp, int N) {
    extern __shared__ double shared_data[];
    // each thread loads one element from global to shared mem (warning: no boundary checks!)
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    shared_data[tid] = i < N ? x[i] : 0;
    __syncthreads();
    // do reduction in shared mem
    for(unsigned int s = 1; s < blockDim.x; s *= 2) {
        if (tid % (2 * s) == 0) {
            shared_data[tid] += shared_data[tid + s];
        }
        __syncthreads();
    }
    // write result for this block to global mem
    if (tid == 0) res_tmp[blockIdx.x] = shared_data[0];
}

__global__ void gpu_vector_sum_1(double *x, double *res_tmp, int N) {
    extern __shared__ double shared_data[];
    // each thread loads one element from global to shared mem (warning: no boundary checks!)
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    shared_data[tid] = i < N ? x[i] : 0;
    __syncthreads();
    // do reduction in shared mem
    for(unsigned int s = 1; s < blockDim.x; s *= 2) {
        int index = 2 * s * tid;
        if (index < blockDim.x) {
            shared_data[index] += shared_data[index + s];
        }
        __syncthreads();
    }
    // write result for this block to global mem
    if (tid == 0) res_tmp[blockIdx.x] = shared_data[0];
}


// Used to sum the values in a warp;
#define WARP_SIZE 32
__inline__ __device__ double warp_reduce(double val) {
    for (int offset = WARP_SIZE / 2; offset > 0; offset /= 2)
        val += __shfl_down_sync(0xFFFFFFFF, val, offset);
    return val;
}

// Note: atomicAdd on double is present only in recent GPU architectures.
// If you don't have it, change the benchmark to use doubles;
__global__ void gpu_vector_sum_2(double *x, double *res, int N) {
    double sum = double(0);
    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < N; i += blockDim.x * gridDim.x) {
        sum += x[i];
    }
    sum = warp_reduce(sum);                    // Obtain the sum of values in the current warp;
    if ((threadIdx.x & (WARP_SIZE - 1)) == 0)  // Same as (threadIdx.x % WARP_SIZE) == 0 but faster
        atomicAdd(res, sum);                   // The first thread in the warp updates the output;
}

//////////////////////////////
//////////////////////////////

// Allocate data on the CPU and GPU;
void VectorSum::alloc() {
    // Compute the number of blocks for implementations where the value is a function of the input size;
    B = (N + block_size - 1) / block_size;
    // Allocate CPU data;
    x = (double*) malloc(sizeof(double) * N);
    // cudaMallocHost(&x, sizeof(double) * N); // You can use cudaMallocHost to get faster transfer speed. Don't use it too much though, it slows down CPU memory;
    res_tmp = (double*) malloc(sizeof(double) * B);
    // Allocate GPU data;
    err = cudaMalloc(&x_d, sizeof(double) * N);
    // The GPU output buffer has size equal to the number of blocks, 
    // as we aggregate partial sums on the CPU;
    err = cudaMalloc(&res_tmp_d, sizeof(double) * B);
}

// Initialize data;
void VectorSum::init() {
    // Just put some values into the array;
    for (int i = 0; i < N; i++) {
        x[i] = double(1) / (i + 1);
    }
}

// Reset the state of the computation after every iteration.
// Reset the result, and transfer data to the GPU if necessary;
void VectorSum::reset() {
    // Reset the result;
    res = 0.0;
    // Transfer data to the GPU;
    cudaMemcpy(x_d, x, sizeof(double) * N, cudaMemcpyHostToDevice);
    // Reset temporary result vector (required only by some implementations);
    cudaMemset(res_tmp_d, 0, sizeof(double) * B);
}

void VectorSum::vector_sum_0(int iter) {
    auto start_tmp = clock_type::now();
    // Call the GPU computation (and set the size of shared memory!);
    gpu_vector_sum_0<<<B, block_size, sizeof(double) * block_size>>>(x_d, res_tmp_d, N);
    int second_block_size = (B + block_size - 1) / block_size; // Do it again, to further reduce the amount of data to move back to CPU;
    if (second_block_size > 0) { // If the input data is small enough, this second step is not necessary;
        gpu_vector_sum_0<<<second_block_size, block_size, sizeof(double) * block_size>>>(res_tmp_d, res_tmp_d, B);
    } else {
        second_block_size = B;
    }

    // Print performance of GPU, not accounting for transfer time;
    if (debug) {
        // Synchronize computation by hand to measure GPU exec. time;
        cudaDeviceSynchronize();
        auto end_tmp = clock_type::now();
        auto exec_time = chrono::duration_cast<chrono::microseconds>(end_tmp - start_tmp).count();
        std::cout << "  pure GPU execution(" << iter << ")=" << double(exec_time) / 1000 << " ms, " << (sizeof(double) * N / (exec_time * 1e3)) << " GB/s" << std::endl;
    }

    // Copy the partial result from the GPU to the CPU;
    cudaMemcpy(res_tmp, res_tmp_d, sizeof(double) * second_block_size, cudaMemcpyDeviceToHost);
    // Sum the partial results using the CPU;
    for (int i = 0; i < second_block_size; i++) {
        res += res_tmp[i];
    }
}

// Second implementation, without warp divergence;
void VectorSum::vector_sum_1(int iter) {
    auto start_tmp = clock_type::now();
    // Call the GPU computation (and set the size of shared memory!);
    gpu_vector_sum_1<<<B, block_size, sizeof(double) * block_size>>>(x_d, res_tmp_d, N);
    int second_block_size = (B + block_size - 1) / block_size; // Do it again, to further reduce the amount of data to move back to CPU;
    if (second_block_size > 0) { // If the input data is small enough, this second step is not necessary;
        gpu_vector_sum_1<<<second_block_size, block_size, sizeof(double) * block_size>>>(res_tmp_d, res_tmp_d, B);
    } else {
        second_block_size = B;
    }

    // Print performance of GPU, not accounting for transfer time;
    if (debug) {
        // Synchronize computation by hand to measure GPU exec. time;
        cudaDeviceSynchronize();
        auto end_tmp = clock_type::now();
        auto exec_time = chrono::duration_cast<chrono::microseconds>(end_tmp - start_tmp).count();
        std::cout << "  pure GPU execution(" << iter << ")=" << double(exec_time) / 1000 << " ms, " << (sizeof(double) * N / (exec_time * 1e3)) << " GB/s" << std::endl;
    }

    // Copy the partial result from the GPU to the CPU;
    cudaMemcpy(res_tmp, res_tmp_d, sizeof(double) * second_block_size, cudaMemcpyDeviceToHost);
    // Sum the partial results using the CPU;
    for (int i = 0; i < second_block_size; i++) {
        res += res_tmp[i];
    }
}

// Third implementation, with grid-stride and shuffle.
// Note: we still use res_tmp_d as output vector,
//   altough this time we use just the first value. 
//   I could have allocated an array of size 1, instead;
void VectorSum::vector_sum_2(int iter) {
    auto start_tmp = clock_type::now();
    // Call the GPU computation;
    gpu_vector_sum_2<<<num_blocks, block_size>>>(x_d, res_tmp_d, N);
    
    // Print performance of GPU, not accounting for transfer time;
    if (debug) {
        // Synchronize computation by hand to measure GPU exec. time;
        cudaDeviceSynchronize();
        auto end_tmp = clock_type::now();
        auto exec_time = chrono::duration_cast<chrono::microseconds>(end_tmp - start_tmp).count();
        std::cout << "  pure GPU execution(" << iter << ")=" << double(exec_time) / 1000 << " ms, " << (sizeof(double) * N / (exec_time * 1e3)) << " GB/s" << std::endl;
    }

    // Copy the partial result from the GPU to the CPU;
    cudaMemcpy(&res, res_tmp_d, sizeof(double), cudaMemcpyDeviceToHost);
}

void VectorSum::execute(int iter) {
    switch (implementation)
    {
    case 0:
        vector_sum_0(iter);
        break;
    case 1:
        vector_sum_1(iter);
        break;
    case 2:
        vector_sum_2(iter);
        break;
    default:
        break;
    }
}

void VectorSum::cpu_validation(int iter) {
    auto start_tmp = clock_type::now();
    cpu_result = 0.0;
    for (int i = 0; i < N; i++) {
        cpu_result += x[i];
    }
    auto end_tmp = clock_type::now();
    auto exec_time = chrono::duration_cast<chrono::microseconds>(end_tmp - start_tmp).count();
    std::cout << "exec time CPU=" << double(exec_time) / 1000 << " ms" << std::endl;
    if (std::abs(res - cpu_result) > 1e-4) std::cout << "result error! GPU=" << res << ", CPU=" << cpu_result << std::endl; 
}

std::string VectorSum::print_result(bool short_form) {
    return std::to_string(res);
}

void VectorSum::clean() {
    free(x);
    free(res_tmp);
    cudaFree(x_d);
    cudaFree(res_tmp_d);
}
