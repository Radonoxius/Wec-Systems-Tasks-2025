#! /bin/bash

cd Task_1/build
clang -O3 ../sequential.c -o sequential

./sequential
rm sequential

clang -fopenmp -O3 ../parallel.c -o parallel

./parallel
rm parallel
cd ../..