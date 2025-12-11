#! /bin/bash

mkdir native/build 2> /dev/null
cd native/build
clang-19 --std=c23 -c ../init.c -Wall -Werror -O3

llvm-ar-19 -rcs libsimulator.a init.o
rm init.o

cd ../..

cargo clean

cargo r --release