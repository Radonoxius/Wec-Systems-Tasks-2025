#! /bin/bash

cd native/build
clang -c ../init.c -g #-O3

ar -rcs libsimulator.a init.o
rm init.o

cd ../..

cargo clean

cargo r #--release