#!/bin/bash

# Run vector sum;
bin/b -d -c -n 100000000 -b vec -I 1 -i 30 -t 64;
bin/b -d -c -n 100000000 -b vec -I 2 -i 30 -t 64;

# Run matrix multiplication;
bin/b -d -c -n 1000 -b mmul -I 1 -i 30 -t 8;
bin/b -d -c -n 1000 -b mmul -I 2 -i 30 -t 8 -B 14;