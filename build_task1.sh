#! /bin/bash

cd Task_1/build
clang -O3 ../sequential.c -o sequential

./sequential
rm sequential
cd ../..