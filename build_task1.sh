#! /bin/bash

mkdir -p Task_1/build
cd Task_1/build

clang -O3 ../sequential.c -o sequential

clang -fopenmp -O3 ../parallel.c -o parallel

cd ../..