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

#include <getopt.h>

#include <cstdlib>
#include <map>
#include <string>

#include "utils.hpp"

//////////////////////////////
//////////////////////////////

#define DEBUG false
#define NUM_ITER 30
#define DEFAULT_BLOCK_SIZE 32
#define DEFAULT_NUM_BLOCKS 64
#define DEFAULT_SKIP 3
#define DEFAULT_BENCHMARK "vec"
#define DEFAULT_NVPROF false
#define DEFAULT_DO_CPU_VALIDATION false

// Options for PPR;
// #define DEFAULT_GRAPH "data/small.mtx"
#define DEFAULT_GRAPH "data/California.mtx"
// #define DEFAULT_GRAPH "data/wikipedia-20070206.mtx"
#define DEFAULT_ALPHA 0.85
#define DEFAULT_MAX_ITER 30
#define DEFAULT_CONVERGENCE 1e-6

//////////////////////////////
//////////////////////////////

enum BenchmarkEnum {
    VEC,
    MMUL,
    PPR,
    ERR
};

//////////////////////////////
//////////////////////////////

inline BenchmarkEnum get_benchmark(std::string benchmark) {
    if (benchmark == "vec")
        return BenchmarkEnum::VEC;
    if (benchmark == "mmul")
        return BenchmarkEnum::MMUL;
    if (benchmark == "ppr")
        return BenchmarkEnum::PPR;
    else
        return BenchmarkEnum::ERR;
}

struct Options {
    // Testing options;
    uint num_iter = NUM_ITER;
    int debug = DEBUG;
    int block_size = DEFAULT_BLOCK_SIZE;
    int num_blocks = DEFAULT_NUM_BLOCKS;
    int N = 0;
    int skip_iterations = DEFAULT_SKIP;
    bool nvprof = DEFAULT_NVPROF;
    BenchmarkEnum benchmark_choice = get_benchmark(DEFAULT_BENCHMARK);
    int implementation = 0;
    int do_cpu_validation = DEFAULT_DO_CPU_VALIDATION;
    std::string graph = DEFAULT_GRAPH;
    double alpha = DEFAULT_ALPHA;
    int maximum_iterations = DEFAULT_MAX_ITER;
    double convergence_threshold = DEFAULT_CONVERGENCE;

    // Used for printing;
    std::map<BenchmarkEnum, std::string> benchmark_map;

    //////////////////////////////
    //////////////////////////////

    Options(int argc, char *argv[]) {
        map_init(benchmark_map)
                (BenchmarkEnum::VEC, "vec")
                (BenchmarkEnum::MMUL, "mmul")
                (BenchmarkEnum::PPR, "ppr");

        int opt;
        static struct option long_options[] = {{"debug", no_argument, 0, 'd'},
                                               {"num_iter", required_argument, 0, 'i'},
                                               {"N", required_argument, 0, 'n'},
                                               {"block_size", required_argument, 0, 't'},
                                               {"num_blocks", required_argument, 0, 'B'},
                                               {"skip_first", required_argument, 0, 's'},
                                               {"benchmark", required_argument, 0, 'b'},
                                               {"nvprof", no_argument, 0, 'v'},
                                               {"implementation", required_argument, 0, 'I'},
                                               {"cpu_validation", no_argument, 0, 'c'},
                                               {"path_to_graph", required_argument, 0, 'g'},
                                               {"alpha", required_argument, 0, 'a'},
                                               {"max_iterations", required_argument, 0, 'm'},
                                               {"convergence_threshold", required_argument, 0, 'e'},
                                               {0, 0, 0, 0}};
        // getopt_long stores the option index here;
        int option_index = 0;

        while ((opt = getopt_long(argc, argv, "di:n:t:B:s:b:vI:cg:a:m:e:", long_options, &option_index)) != EOF) {
            switch (opt) {
                case 'd':
                    debug = true;
                    break;
                case 'i':
                    num_iter = atoi(optarg);
                    break;
                case 'n':
                    N = atoi(optarg);
                    break;
                case 't':
                    block_size = atoi(optarg);
                    break;
                case 'B':
                    num_blocks = atoi(optarg);
                    break;
                case 's':
                    skip_iterations = atoi(optarg);
                    break;
                case 'b':
                    benchmark_choice = get_benchmark(optarg);
                    break;
                case 'v':
                    nvprof = true;
                    break;
                case 'I':
                    implementation = atoi(optarg);
                    break;
                case 'c':
                    do_cpu_validation = true;
                    break;
                case 'g':
                    graph = optarg;
                    break;
                case 'a':
                    alpha = atof(optarg);
                    break;
                case 'm':
                    maximum_iterations = atoi(optarg);
                    break;
                case 'e':
                    convergence_threshold = atof(optarg);
                    break;
                default:
                    break;
            }
        }
    }
};