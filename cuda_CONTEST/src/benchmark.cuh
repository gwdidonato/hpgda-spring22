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

#pragma once
#include <chrono>
#include <iostream>
#include <string>

#include "options.hpp"
#include "utils.hpp"

struct Benchmark {
   public:
    virtual void alloc() = 0;
    virtual void init() = 0;
    virtual void reset() = 0;
    virtual void execute(int iter);
    virtual void clean();
    virtual std::string print_result(bool short_form = false) = 0;
    virtual void cpu_validation(int iter);
    void run();

    Benchmark(Options &options) : debug(options.debug),
                                  num_executions(options.num_iter),
                                  N(options.N),
                                  block_size(options.block_size),
                                  num_blocks(options.num_blocks),
                                  implementation(options.implementation),
                                  skip_iterations(options.skip_iterations),
                                  benchmark_name(options.benchmark_choice),
                                  do_cpu_validation(options.do_cpu_validation),
                                  nvprof(options.nvprof) {
        cudaDeviceGetAttribute(&pascalGpu, cudaDeviceAttr::cudaDevAttrConcurrentManagedAccess, 0);
        if (debug) {
            std::cout << "------------------------------" << std::endl;
            std::cout << "- running " << options.benchmark_map[benchmark_name] << std::endl;
            std::cout << "- num executions=" << num_executions << std::endl;
            std::cout << "- iterations to skip=" << skip_iterations << std::endl;
            std::cout << "- N=" << N << std::endl;
            std::cout << "- implementation number to use (where applicable)=" << implementation << std::endl;
            std::cout << "- block size (1D)=" << block_size << std::endl;
            std::cout << "- num blocks (1D, where applicable)=" << num_blocks << std::endl;
            std::cout << "- nvprof support=" << nvprof << std::endl;
            std::cout << "- do cpu validation=" << do_cpu_validation << std::endl;
            std::cout << "------------------------------" << std::endl;
        }
    }

    virtual ~Benchmark(){};

   protected:
    int debug = DEBUG;
    int num_executions = NUM_ITER;
    int N = 0;
    int block_size = DEFAULT_BLOCK_SIZE;
    int num_blocks = DEFAULT_NUM_BLOCKS;
    int skip_iterations = 0;
    int pascalGpu = 0;
    int implementation = 0;
    bool nvprof = DEFAULT_NVPROF;
    bool do_cpu_validation = DEFAULT_DO_CPU_VALIDATION;
    BenchmarkEnum benchmark_name;
    int err = 0;
};