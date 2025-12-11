#! /bin/bash

cd Task_3/simulator

mkdir native/build 2> /dev/null
cd native/build
clang --std=c23 -c ../init.c -Wall -Werror -O3

llvm-ar -rcs libsimulator.a init.o
rm init.o

cd ../..

cargo b --release

cd ../..