#! /bin/bash

cd native/build
clang-19 --std=c23 -c ../init.c -g #-O3

llvm-ar-19 -rcs libsimulator.a init.o
rm init.o

cd ../..

cargo clean

cargo r #--release